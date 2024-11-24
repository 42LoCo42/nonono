#include <arpa/inet.h>
#include <err.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

int main(int argc, char** argv) {
	if(argc < 3) errx(1, "Usage: %s <ip> <port>", argv[0]);
	const char* group = argv[1];
	const int   port  = atoi(argv[2]);

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0) err(1, "could not create socket");

	int yes = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		err(1, "could not set SO_REUSEADDR");

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port),
	};
	if(bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		err(1, "could not bind to %d", port);

	struct ip_mreqn req = {
		.imr_multiaddr.s_addr = inet_addr(group),
		.imr_address.s_addr = htonl(INADDR_ANY),
		.imr_ifindex = 0,
	};
	if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req)) < 0)
		err(1, "could not join multicast group %s", group);

	for(;;) {
		char buf[BUFSIZE + 1];
		socklen_t addrlen = sizeof(addr);
		ssize_t nbytes = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr*) &addr, &addrlen);
		if(nbytes < 0) err(1, "recvfrom");
		buf[nbytes] = 0;
		printf("%s:%d: %s%s",
			inet_ntoa(addr.sin_addr),
			addr.sin_port,
			buf,
			buf[nbytes - 1] == '\n' ? "" : "\n"
		);
		fflush(stdout);
	}
}
