#include "wrappers.h"

void _attach(Ixp9Req* req) {
	attach(req);
}

void _clunk(Ixp9Req* req) {
	clunk(req);
}

void _create(Ixp9Req* req) {
	create(req);
}

void _flush(Ixp9Req* req) {
	flush(req);
}

void _open(Ixp9Req* req) {
	open(req);
}

void _read(Ixp9Req* req) {
	read(req);
}

void _remove(Ixp9Req* req) {
	remove(req);
}

void _stat(Ixp9Req* req) {
	stat(req);
}

void _walk(Ixp9Req* req) {
	walk(req);
}

void _write(Ixp9Req* req) {
	write(req);
}

void _wstat(Ixp9Req* req) {
	wstat(req);
}

void _freefid(IxpFid* fid) {
	freefid(fid);
}
