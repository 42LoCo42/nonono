#define NNG_ELIDE_DEPRECATED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/supplemental/tls/tls.h>

#include "utils.h"
#include "cert"
#include "key"

// ----------------------------------------------------------//

#define LISTEN_CERT "ws://0.0.0.0:37812"
#define LISTEN_DATA "wss://0.0.0.0:25565"
#define DIAL_CERT   "ws://42loco42.duckdns.org:37812"
#define DIAL_DATA   "wss://42loco42.duckdns.org:25565"

// --------------------------------------------------------

void client() {
	int rv;

	nng_socket cert_socket = NNG_SOCKET_INITIALIZER;
	nng_socket data_socket = NNG_SOCKET_INITIALIZER;
	nng_dialer data_dialer = NNG_DIALER_INITIALIZER;
	nng_tls_config* tls_config = NULL;

	char* recv_cert = NULL;
	size_t recv_cert_len = 0;

	char* recv_data = NULL;
	size_t recv_data_len = 0;

	printf("open cert\n");
	nng_checked(nng_req0_open(&cert_socket));

	printf("recv cert\n");
	nng_checked(nng_dial(cert_socket, DIAL_CERT, NULL, 0));
	nng_checked(nng_send(cert_socket, "", 0, 0));
	nng_checked(nng_recv(cert_socket, &recv_cert, &recv_cert_len, NNG_FLAG_ALLOC));

	printf("open data\n");
	nng_checked(nng_req0_open(&data_socket));
	nng_checked(nng_dialer_create(&data_dialer, data_socket, DIAL_DATA));

	printf("config\n");
	nng_checked(nng_tls_config_alloc(&tls_config, NNG_TLS_MODE_CLIENT));
	nng_checked(nng_tls_config_ca_chain(tls_config, recv_cert, NULL));
	nng_checked(nng_dialer_set_ptr(data_dialer, NNG_OPT_TLS_CONFIG, tls_config));

	printf("recv data\n");
	nng_checked(nng_dialer_start(data_dialer, 0));
	nng_checked(nng_send(data_socket, "from client", 11, 0));
	nng_checked(nng_recv(data_socket, &recv_data, &recv_data_len, NNG_FLAG_ALLOC));
	printf("got %s\n", recv_data);

end:
	if(recv_data) nng_free(recv_data, recv_data_len);
	if(recv_cert) nng_free(recv_cert, recv_cert_len);
	if(tls_config) nng_tls_config_free(tls_config);
	nng_close(data_socket);
	nng_close(cert_socket);
}

// --------------------------------------------------------

void certCB(CallbackArg* arg, nng_msg* received) {
	(void) received;
	printf("handling cert request\n");
	nng_send(*arg->socket, (void*) cert, cert_len, 0);
}

void dataCB(CallbackArg* arg, nng_msg* received) {
	printf("recv %s\n", (char*) nng_msg_body(received));

	const char response[] = "hello world";
	const size_t response_len = sizeof(response);
	printf("send %s\n", response);
	nng_send(*arg->socket, (void*) response, response_len, 0);
}

void server() {
	int rv;

	nng_socket socket = NNG_SOCKET_INITIALIZER;
	nng_listener cert_listener = NNG_LISTENER_INITIALIZER;
	nng_listener data_listener = NNG_LISTENER_INITIALIZER;
	nng_tls_config* tls_config = NULL;

	CallbackArg cert_arg = {.realCallback = certCB, .aio = NULL, .socket = &socket};
	CallbackArg data_arg = {.realCallback = dataCB, .aio = NULL, .socket = &socket};

	printf("open\n");
	nng_checked(nng_rep0_open(&socket));
	nng_checked(nng_listener_create(&cert_listener, socket, LISTEN_CERT));
	nng_checked(nng_listener_create(&data_listener, socket, LISTEN_DATA));

	printf("config\n");
	nng_checked(nng_tls_config_alloc(&tls_config, NNG_TLS_MODE_SERVER));
	nng_checked(nng_tls_config_own_cert(tls_config, (const char*) cert, (const char*) key, NULL));
	nng_checked(nng_listener_set_ptr(data_listener, NNG_OPT_TLS_CONFIG, tls_config));

	printf("listen\n");
	nng_checked(nng_aio_alloc(&cert_arg.aio, baseCallback, &cert_arg));
	nng_checked(nng_aio_alloc(&data_arg.aio, baseCallback, &data_arg));
	nng_checked(nng_listener_start(cert_listener, 0));
	nng_checked(nng_listener_start(data_listener, 0));

	printf("recv\n");
	nng_recv_aio(socket, cert_arg.aio);
	nng_aio_wait(cert_arg.aio);
	nng_recv_aio(socket, data_arg.aio);
	nng_aio_wait(data_arg.aio);

end:
	if(data_arg.aio) nng_aio_free(data_arg.aio);
	if(cert_arg.aio) nng_aio_free(cert_arg.aio);
	if(tls_config) nng_tls_config_free(tls_config);
	nng_close(socket);
}

// ------------------------------

int main(int argc, char** argv) {
	(void) argv;
	atexit(nng_fini);

	if(argc > 1) {
		client();
	} else {
		server();
	}
}
