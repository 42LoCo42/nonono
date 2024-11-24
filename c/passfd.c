#include <stdio.h>
#include <netinet/ip.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define breakpoint raise(SIGINT)
#define check(expr, name) if(expr) {perror(name); goto end;}

struct sockaddr_un addr = {
	.sun_family = AF_UNIX,
	.sun_path = "foo",
};

int send_fd(int socket, int fd) {
	char c_buf[CMSG_SPACE(sizeof(fd))] = {};
	struct iovec io = {
		.iov_base = "*",
		.iov_len = 1,
	};
	struct msghdr msg = {
		.msg_iov = &io,
		.msg_iovlen = 1,
		.msg_control = c_buf,
		.msg_controllen = sizeof(c_buf),
	};
	struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*((int*) CMSG_DATA(cmsg)) = fd;

	return sendmsg(socket, &msg, 0);
}

int recv_fd(int socket) {
	char m_buf[1] = {};
	char c_buf[CMSG_SPACE(sizeof(int))] = {};
	struct iovec io = {
		.iov_base = m_buf,
		.iov_len = sizeof(m_buf),
	};
	struct msghdr msg = {
		.msg_iov = &io,
		.msg_iovlen = 1,
		.msg_control = c_buf,
		.msg_controllen = sizeof(c_buf),
	};

	return recvmsg(socket, &msg, 0) < 0 ? -1 : *((int*) CMSG_DATA(CMSG_FIRSTHDR(&msg)));
}

void sender() {
	printf("sender\n");

	int s = -1, c = -1;
	struct sockaddr_in listen_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(12345),
		.sin_addr = 0,
	};

	check((s = socket(AF_INET, SOCK_STREAM, 0)) < 0, "socket");
	check(bind(s, (struct sockaddr*) &listen_addr, sizeof(listen_addr)) < 0, "bind");
	check(listen(s, 1) < 0, "listen");
	check((c = accept(s, NULL, NULL)) < 0, "accept");

	close(s);

	check((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0, "socket");
	check(connect(s, (struct sockaddr*) &addr, sizeof(addr)) < 0, "connect");
	check(send_fd(s, c) < 0, "send_fd");

end:
	if(c != -1) close(c);
	if(s != -1) close(s);
}

void receiver() {
	printf("receiver\n");

	int s = -1, c = -1, fd = -1;

	check((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0, "socket");
	check(bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0, "bind");
	check(listen(s, 1) < 0, "listen");
	check((c = accept(s, NULL, NULL)) < 0, "accept");

	check((fd = recv_fd(c)) < 0, "recv_fd");
	check(write(fd, "from receiver\n", 14) < 0, "write");

end:
	if(c != -1) close(c);
	if(s != -1) close(s);
}

int main(int argc, char** argv) {
	(void*) argv;

	if(argc == 2) {
		sender();
	} else {
		receiver();
	}
}
