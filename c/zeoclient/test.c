#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main() {
	struct addrinfo* results = NULL;
	int ret = getaddrinfo("192.168.54.75", "37812", NULL, &results);
	for(struct addrinfo* i = results; i != NULL; i = i->ai_next) {
		char buf[6 + INET6_ADDRSTRLEN] = {0};
		void* src = NULL;
		switch(i->ai_family) {
			case AF_INET:  src = &((struct sockaddr_in*)  i->ai_addr)->sin_addr;  break;
			case AF_INET6: src = &((struct sockaddr_in6*) i->ai_addr)->sin6_addr; break;
			default:
				warnx("Unknown address family");
				continue;
		}
		inet_ntop(i->ai_family, src, buf + 6, sizeof(buf) - 6);
		strncpy(buf, i->ai_socktype == SOCK_STREAM ? "tcp://" : "udp://", 6);
		puts(buf);
	}
}
