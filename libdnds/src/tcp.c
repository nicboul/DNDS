/*
 * Dynamic Network Directory Service
 * Copyright (C) 2009-2013
 * Nicolas J. Bouliane <nib@dynvpn.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "logger.h"
#include "netbus.h"
#include "tcp.h"

#define CONN_BACKLOG 512

#define TCPBUS_SERVER 0x1
#define TCPBUS_CLIENT 0x2

#define NUM_EVENTS 64
#define BACKING_STORE 512

#define ION_READ 1
#define ION_WRTE 2
#define ION_EROR 3

static int tcpbus_queue = -1;
static struct epoll_event ep_ev[NUM_EVENTS];

static int setnonblocking(int socket)
{
	int ret;
	ret = fcntl(socket, F_GETFL);
	if (ret >= 0) {
		ret = fcntl(socket, F_SETFL, ret | O_NONBLOCK);
	}
	return ret;
}

static int setreuse(int socket)
{
	int ret, on = 1;
	ret = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	return ret;
}

static void tcpbus_disconnect(peer_t *peer)
{
	int ret;

	//close() will cause the socket to be automatically removed from the queue
	ret = close(peer->socket);
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> close() failed: %u %u %s :: %s:%i",
			peer->socket, ret, strerror(errno), __FILE__, __LINE__);
		return;
	}

	jlog(L_DEBUG, "tcpbus]> client close: %u", peer->socket);

	free(peer->buffer);
	free(peer);
}

static int tcpbus_send(peer_t *peer, void *data, int len)
{
	int ret = 0;
	int total = 0;
	int byteleft = len;

	errno = 0;
	while (total < len) {
		ret = send(peer->socket, (uint8_t*)data + total, byteleft, 0);

		if (errno != 0)
			jlog(L_ERROR, "tcpbus]> tcpbus_send errno [%i] :: %s:%i", __FILE__, __LINE__);

		if (ret == -1 && errno != 11)
			return -1;

		if (ret != -1) {
			total += ret;
			byteleft -= ret;
		}
	}

	return ret;
}

static int tcpbus_recv(peer_t *peer)
{
// TODO use dynamic buffer
#define PEER_BUF_SZ 5000
	fd_set rfds;
	struct timeval tv;
	int ret = 0;
	char tmpbuf[5000];

	FD_ZERO(&rfds);
	FD_SET(peer->socket, &rfds);

	/* Wait up to one second */
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	ret = select(peer->socket + 1, &rfds, NULL, NULL, &tv);
	if (ret == 0) { /* TIMEOUT !*/
		jlog(L_NOTICE, "tcpbus]> tcpbus_recv() TIMEOUT peer->socket(%i) :: %s:%i",
			peer->socket, __FILE__, __LINE__);
		return -1;
	}

	if (peer->buffer == NULL) {
		peer->buffer = calloc(1, PEER_BUF_SZ);
		if (peer->buffer == NULL) {
			jlog(L_ERROR, "tcpbus]> tcpbus_recv() calloc FAILED :: %s:%i", __FILE__, __LINE__);
			return -1;
		}
	}
	else {
		memset(peer->buffer, 0, PEER_BUF_SZ);
	}

	/*
	 * XXX It may happen that the buffer is too short,
	 * in that case we should then re-alloc and move
	 * the bytes.
	 */
	ret = recv(peer->socket, peer->buffer, PEER_BUF_SZ, 0);
	printf("ret: %d\n", ret);

	if (ret < 0)
		return -1;

	return ret;
}

static void tcpbus_on_input(peer_t *peer)
{
	jlog(L_DEBUG, "tcpbus]> received data for peer {%i}", peer->socket);

	if (peer->on_input)
		peer->on_input(peer);
}

static void tcpbus_on_disconnect(peer_t *peer)
{
	// inform upper layer
	if (peer->on_disconnect)
		peer->on_disconnect(peer);

	tcpbus_disconnect(peer);
}

static void tcpbus_on_connect(peer_t *peer)
{
	int ret, addrlen;
	struct sockaddr_in addr;
	peer_t *npeer; /* new peer connected */

	npeer = calloc(sizeof(peer_t), 1);

	addrlen = sizeof(struct sockaddr_in);
	npeer->socket = accept(peer->socket, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
	if (npeer->socket < 0) {
		jlog(L_ERROR, "tcpbus]> accept() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		free(npeer);
		return;
	}

	ret = setnonblocking(npeer->socket);
        if (ret < 0) {
                jlog(L_ERROR, "tcpbus]> setnonblocking() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		free(npeer);
                return;
        }

	npeer->type = TCPBUS_CLIENT;
	npeer->on_connect = peer->on_connect;
	npeer->on_disconnect = peer->on_disconnect;
	npeer->on_input = peer->on_input;
	npeer->recv = peer->recv;
	npeer->send = peer->send;
	npeer->disconnect = peer->disconnect;
	npeer->ext_ptr = peer->ext_ptr;
	npeer->buffer = NULL;

	ret = tcpbus_ion_add(npeer->socket, npeer);
	if (ret < 0) {
		jlog(L_ERROR, "tcpbus]> tcpbus_ion_add() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		free(npeer);
	}

	if (peer->on_connect)
		peer->on_connect(npeer);

	jlog(L_DEBUG, "tcpbus]> successfully added TCP client {%i} on server {%i}", npeer->socket, peer->socket);
}

int tcpbus_server(const char *in_addr,
		   char *port,
		   void (*on_connect)(peer_t*),
		   void (*on_disconnect)(peer_t*),
		   void (*on_input)(peer_t*),
		   void *ext_ptr)
{
	int ret;
	struct sockaddr_in addr;
	peer_t *peer;

	jlog(L_NOTICE, "tcpbus]> server [%s:%s]\n", in_addr, port);

	peer = calloc(sizeof(peer_t), 1);
	peer->type = TCPBUS_SERVER;
	peer->on_connect = on_connect;
	peer->on_disconnect = on_disconnect;
	peer->on_input = on_input;
	peer->recv = tcpbus_recv;
	peer->send = tcpbus_send;
	peer->disconnect = tcpbus_disconnect;
	peer->buffer = NULL;
	peer->ext_ptr = ext_ptr;

	peer->socket = socket(PF_INET, SOCK_STREAM, 0);
	if (peer->socket < 0) {
		jlog(L_NOTICE, "tcpbus]> socket() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(in_addr);

	ret = setreuse(peer->socket);
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> setreuse() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return -1;
	}

	ret = bind(peer->socket, (const struct sockaddr *)&addr, sizeof(const struct sockaddr));
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> bind() %s %s :: %s:%i", strerror(errno), in_addr, __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return -1;
	}

	/* The backlog parameter defines the maximum length the
	 * queue of pending connection may grow to. LISTEN(2)
	 */
	ret = listen(peer->socket, CONN_BACKLOG);
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> set_nonblocking() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return -1;
	}

	ret = tcpbus_ion_add(peer->socket, peer);
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> tcpbus_ion_add() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return -1;
	}

	return 0;
}

peer_t *tcpbus_client(const char *addr,
			  const char *port,
			  void (*on_disconnect)(peer_t*),
			  void (*on_input)(peer_t*))
{
	fd_set wfds;
	struct timeval tv;
	int ret = 0;
	int optval = 0;
	socklen_t optlen = 0;
	struct sockaddr_in addr_in;
	peer_t *peer = NULL;

	FD_ZERO(&wfds);
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	peer = calloc(sizeof(peer_t), 1);

	peer->socket = socket(PF_INET, SOCK_STREAM, 0);
	if (peer->socket == -1) {
		jlog(L_ERROR, "tcpbus]> socket() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return NULL;
	}

        ret = setnonblocking(peer->socket);
        if (ret < 0) {
                jlog(L_ERROR, "tcpbus]> setnonblocking() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
                free(peer);
                return NULL;
        }

	memset(&addr_in, 0, sizeof(struct sockaddr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(atoi(port));
	addr_in.sin_addr.s_addr = inet_addr(addr);

	FD_SET(peer->socket, &wfds);
	errno = 0;

	ret = connect(peer->socket, (const struct sockaddr *)&addr_in, sizeof(const struct sockaddr));
	if (ret == -1) {
		if (errno == EINPROGRESS) {
			/* The socket is non-blocking and the
			 * connection cannot be completed immediately.
			 */
			ret = select(peer->socket + 1, NULL, &wfds, NULL, &tv);
			if (ret == 0) { /* TIMEOUT */
				jlog(L_DEBUG, "tcpbus]> connect() timed out :: %s:%i", __FILE__, __LINE__);
				close(peer->socket);
				free(peer);
				return NULL;
			}
			else {
				optlen = (socklen_t)sizeof(optval);
				/* use getsockopt(2) with SO_ERROR to check for error conditions */
				ret = getsockopt(peer->socket, SOL_SOCKET, SO_ERROR, &optval, &optlen);
				if (ret == -1) {
					jlog(L_DEBUG, "tcpbus]> getsockopt() %s :: %s:%i", __FILE__, __LINE__);
					close(peer->socket);
					free(peer);
					return NULL;
				}

				if (optval != 0) { /* NOT CONNECTED ! TIMEOUT... */
					jlog(L_DEBUG, "tcpbus]> connect() timed out :: %s:%i", __FILE__, __LINE__);
					close(peer->socket);
					free(peer);
	                                return NULL;
				}
				else {
					/* ... connected, we continue ! */
				}
			}

		}
		else {
			jlog(L_DEBUG, "tcpbus]> connect() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
			close(peer->socket);
			free(peer);
			return NULL;
		}
	}

	peer->type = TCPBUS_CLIENT;
	peer->on_disconnect = on_disconnect;
	peer->on_input = on_input;
	peer->send = tcpbus_send;
	peer->recv = tcpbus_recv;
	peer->disconnect = tcpbus_disconnect;
	peer->buffer = NULL;

	ret = tcpbus_ion_add(peer->socket, peer);
	if (ret < 0) {
		jlog(L_NOTICE, "tcpbus]> ion_add() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		close(peer->socket);
		free(peer);
		return NULL;
	}

	return peer;
}

int tcpbus_init()
{
	jlog(L_NOTICE, "tcpbus]> init");
	tcpbus_queue = tcpbus_ion_new();
}


int tcpbus_ion_poke(int queue, void (*notify)(void *ud))
{
	int nfd, i;
	peer_t *peer = NULL;

	nfd = epoll_wait(tcpbus_queue, ep_ev, NUM_EVENTS, 1);
	if (nfd < 0) {
		jlog(L_NOTICE, "ion]> epoll_wait() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);
		return -1;
	}

	for (i = 0; i < nfd; i++) {

		peer = ep_ev[i].data.ptr;

		if (ep_ev[i].events & EPOLLRDHUP) {
			tcpbus_on_disconnect(peer);

		} else if (ep_ev[i].events & EPOLLERR) {
			tcpbus_on_disconnect(peer);

		} else if (ep_ev[i].events & EPOLLIN) {

			if (peer->type == TCPBUS_SERVER) {
				tcpbus_on_connect(peer);

			} else if (peer->type == TCPBUS_CLIENT) {
				tcpbus_on_input(peer);
			}
		}
	}

	return nfd;
}

int tcpbus_ion_add(int fd, void *data)
{
	struct epoll_event nevent;
	int ret;

	nevent.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	nevent.data.ptr = data;

	ret = epoll_ctl(tcpbus_queue, EPOLL_CTL_ADD, fd, &nevent);
	if (ret < 0)
		jlog(L_NOTICE, "ion]> epoll_ctl() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);

	return ret;
}

int tcpbus_ion_new()
{
	int epoll_fd;

	epoll_fd = epoll_create(BACKING_STORE);
	if (epoll_fd <	0)
		jlog(L_NOTICE, "ion]> epoll_create() %s :: %s:%i", strerror(errno), __FILE__, __LINE__);

	return epoll_fd;
}

