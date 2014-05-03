
#include <netbus.h>
//#include <udt.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
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
*/

void test_on_disconnect(netc_t *netc)
{
	(void)(netc);
	printf("on disconnect\n");
}

void test_on_input(netc_t *netc)
{
	(void)(netc);
	printf("on input\n");
}

void test_on_secure(netc_t *netc)
{
	(void)(netc);
	printf("on secure\n");
}

static void *bus_loop(void *ptr)
{
	(void)(ptr);

	while (1) {
		udtbus_poke_queue();
	}
	return NULL;
}

int test_init()
{
	netbus_init();
/*
	peer_t *n_peer = NULL;
	n_peer = udtbus_client("127.0.0.1", "9090", test_on_disconnect, test_on_input);
*/
	netc_t *n_netc;
	n_netc = net_client("192.168.1.100", "9090", NET_PROTO_UDT, NET_UNSECURE, NULL,
			test_on_disconnect, test_on_input, test_on_secure);

	if (n_netc == NULL) {
		printf("n_netc is null\n");
		return -1;
	}


	int size = 1500;
//	int ssize = 0;
//	int ss = 0;
	int i = 0;
	char *data = malloc(size);
/*
	pthread_t thread_loop;
	pthread_create(&thread_loop, NULL, bus_loop, NULL);
*/
        DNDSMessage_t *msg = NULL;

	for (i = 0; i < 1000000; i++) {

		DNDSMessage_new(&msg);
		DNDSMessage_set_channel(msg, 0);
		DNDSMessage_set_pdu(msg, pdu_PR_ethernet);
		DNDSMessage_set_ethernet(msg, (uint8_t*)data, 1500);

		net_send_msg(n_netc, msg);
		DNDSMessage_set_ethernet(msg, NULL, 0);
		//DNDSMessage_del(msg);

/*
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
*/
	}
	
	return 0;
}

