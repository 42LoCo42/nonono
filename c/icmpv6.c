#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define err(x) {perror(x); return 1;}

void usage() {
	fprintf(stderr, "Usage: <IPv6 address> [ttl] [interface ID] [flowinfo]\n");
}

int strtoi(char* str) {
	char* endptr;
	int res = strtol(str, &endptr, 0);
	if(*str != 0 && *endptr == 0) {
		return res;
	} else {
		fprintf(stderr, "strtol: wrong input: %s\n", endptr);
		exit(1);
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		usage();
		return 1;
	}

	int s;
	if((s = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) err("socket");

	char* endptr;
	int ttl = argc >= 3 ? strtoi(argv[2]) : -1;

	if(setsockopt(s, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &ttl, sizeof(ttl)) < 0) err("setsockopt");

	struct sockaddr_in6 sin6 = {
		.sin6_family = AF_INET6,
		.sin6_port = 0,
		.sin6_scope_id = argc >= 4 ? strtoi(argv[3]) : 0,
		.sin6_flowinfo = argc >= 5 ? strtoi(argv[4]) : 0,
	};
	if(inet_pton(AF_INET6, argv[1], &sin6.sin6_addr) != 1) err("inet_pton 6");

	char payload[] = {
		0x80, 0x00, 0x27, 0x26,
		0x00, 0x00, 0x00, 0x00,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
		42, 42, 42, 42, 42, 42, 42, 42,
	};

	char response[64] = {};

	if(sendto(s, payload, sizeof(payload), 0, (const struct sockaddr*) &sin6, sizeof(sin6)) < 0) err("sendto");
	if(recvfrom(s, response, sizeof(response), 0, NULL, NULL) < 0) err("recvfrom");
	write(STDOUT_FILENO, response, sizeof(response));
	putchar('\n');

	close(s);
	return 0;
}
