#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int init(const char* addr, const char* port);
int mySend(const char* msg, uint32_t len);
int myRecv(char** msg, uint32_t* len);

int main(int argc, char** argv) {
	if(argc < 3) err(1, "Usage: %s address port", argv[0]);

	if(init("localhost", "37812") < 0) err(1, "Could not init zeoclient");

	if(mySend("foo", 3) < 0) err(1, "Could not send");

	init("192.168.178.20", "37812");
	mySend("bar", 3);

	char*    buf = NULL;
	uint32_t len = 0;
	if(myRecv(&buf, &len) < 0)
		err(1, "Could not receive");

	write(1, buf, len);
	free(buf);
}
