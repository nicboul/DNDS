
#include <netbus.h>
#include <udt.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void *bus_loop(void *ptr)
{
	(void)(ptr);

	while (1) {
		udtbus_poke_queue();
	}
	return NULL;
}

void test_on_disconnect(peer_t *peer)
{
	(void)(peer);
	return;
}

void test_on_input(peer_t *peer)
{
	(void)(peer);
	return;
}

int test_init()
{
	peer_t *n_peer = NULL;

	netbus_init();

	n_peer = udtbus_client("127.0.0.1", "9090", test_on_disconnect, test_on_input);

	int size = 100000;
	int ssize = 0;
	int ss = 0;
	int i = 0;
	char *data = malloc(size);

	pthread_t thread_loop;
	pthread_create(&thread_loop, NULL, bus_loop, NULL);


	for (i = 0; i < 1000000; i++) {
		ssize = 0;
		ss = 0;
		while (ssize < size) {
			if (n_peer->send(n_peer, data + ssize, size - ssize) == -1) {
				printf("send() problem\n");
				return -1;
			}

			ssize += ss;
		}

		if (ssize < size)
			break;
	}
	
	return 0;
}

