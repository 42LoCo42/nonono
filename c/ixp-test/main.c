#include <err.h>
#include <ixp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

void fs_attach(Ixp9Req* r) {
	puts("attach");

	r->fid->qid.type = P9_QTDIR;
	r->fid->qid.path = (uintptr_t)r->fid;
	r->ofcall.rattach.qid = r->fid->qid;
	ixp_respond(r, NULL);
}

void fs_stat(Ixp9Req* r) {
	
}

int main() {
	int fd = ixp_announce("tcp!localhost!37812");
	check(fd < 0, "Could not listen");

	IxpServer srv  = {0};
	Ixp9Srv   srv9 = {0};
	srv9.attach = fs_attach;
	srv9.stat   = fs_stat;
	ixp_listen(&srv, fd, &srv9, ixp_serve9conn, NULL);
	// ixp_listen(&srv, fd, NULL, server, NULL);
	check(ixp_serverloop(&srv) != 0, "Could not start server loop");
}
