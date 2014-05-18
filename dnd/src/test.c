
//#include <netbus.h>
#include <udt.h>

#include <pthread.h>


#include <signal.h>
void sig_handler(int sig)
{
        exit(0);
}



void test_on_input(peer_t *peer)
{
	(void)(peer);
        peer->buffer_data_len = peer->recv(peer);
	//peer->buffer
}

void test_on_disconnect(peer_t *peer)
{

	(void)(peer);
}

void test_on_connect(peer_t *peer)
{
	(void)(peer);
	printf("connect!\n");
}


/*
netc_t *g_netc = NULL;
void test_on_connect(netc_t *netc)
{
	(void)(netc);
	g_netc = netc;
}

void test_on_secure(netc_t *netc)
{
	(void)(netc);
}

void test_on_disconnect(netc_t *netc)
{
	(void)(netc);
}

void test_on_input(netc_t *netc)
{
	(void)(netc);
        
}
*/
static void *bus_loop(void *ptr)
{
	(void)(ptr);

	while (1) {
		udtbus_poke_queue();
	}

	return NULL;
}


int init_test()
{

        signal(SIGINT, sig_handler);

//	int size = 5;
//      char *data = malloc(size);
//	int ret = 0;

	udtbus_server("192.168.1.100", "9090", test_on_connect, test_on_disconnect, test_on_input, NULL);

//	net_server("192.168.1.100", "9090", NET_PROTO_UDT, NET_UNSECURE, NULL,
//			test_on_connect, test_on_disconnect, test_on_input, test_on_secure);


	pthread_t thread_loop;
	pthread_create(&thread_loop, NULL, bus_loop, NULL);

	while (1) { sleep(1); }


/*
	while (1) {
		if (g_netc) {
		DNDSMessage_t *msg = NULL;

		DNDSMessage_new(&msg);
		DNDSMessage_set_channel(msg, 0);
		DNDSMessage_set_pdu(msg, pdu_PR_ethernet);
		DNDSMessage_set_ethernet(msg, (uint8_t*)data,5);

		ret = net_send_msg(g_netc, msg);
		DNDSMessage_set_ethernet(msg, NULL, 0);
		DNDSMessage_del(msg);
		} else { sleep(1); };
	}
*/
	return 0;
}

