#include <err.h>
#include <fcntl.h>
#include <sodium.h>
#include <stdio.h>
#include <unistd.h>

#define error err(1, "%s:%d", __FILE__, __LINE__)

#define THINGS \
crypto_secretstream_xchacha20poly1305_state state; \
unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]; \
unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES]; \
unsigned char mbuf[BUF_LEN]; \
unsigned char cbuf[BUF_LEN + crypto_secretstream_xchacha20poly1305_ABYTES];

const size_t BUF_LEN = 1024;

int readall(int fd, void* buf, size_t count) {
	size_t done = 0;
	while(done < count) {
		ssize_t now = read(fd, buf + done, count - done);
		if(now < 1) return now;
		done += now;
	}
	return 1;
}

int writeall(int fd, const void* buf, size_t count) {
	size_t done = 0;
	while(done < count) {
		ssize_t now = write(fd, buf + done, count - done);
		if(now < 1) return now;
		done += now;
	}
	return 1;
}

void encrypt() {
	THINGS

	crypto_secretstream_xchacha20poly1305_keygen(key);
	if(crypto_secretstream_xchacha20poly1305_init_push(&state, header, key) < 0) error;

	if(writeall(STDERR_FILENO, key, crypto_secretstream_xchacha20poly1305_KEYBYTES) < 1) error;
	close(STDERR_FILENO);

	if(writeall(STDOUT_FILENO, header, crypto_secretstream_xchacha20poly1305_HEADERBYTES) < 1) error;

	for(;;) {
		ssize_t bytes = read(STDIN_FILENO, mbuf, BUF_LEN);
		if(bytes < 1) return;
		crypto_secretstream_xchacha20poly1305_push(&state, cbuf, NULL, mbuf, bytes, NULL, 0, 0);
		if(writeall(STDOUT_FILENO, &bytes, sizeof(bytes)) < 1) error;
		if(writeall(STDOUT_FILENO, cbuf, bytes + crypto_secretstream_xchacha20poly1305_ABYTES) < 1) error;
	}
}

void decrypt(const char* cipherpath) {
	THINGS

	int cipherfile = open(cipherpath, O_RDONLY, 0);
	if(cipherfile < 0) error;

	if(readall(STDIN_FILENO, key, crypto_secretstream_xchacha20poly1305_KEYBYTES) < 1) error;

	if(readall(cipherfile, header, crypto_secretstream_xchacha20poly1305_HEADERBYTES) < 1) error;
	if(crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key) < 0) error;

	for(;;) {
		size_t mbytes;
		int ret = readall(cipherfile, &mbytes, sizeof(mbytes));
		if(ret < 0) error; else if(ret == 0) return;

		size_t cbytes = mbytes + crypto_secretstream_xchacha20poly1305_ABYTES;
		if(readall(cipherfile, cbuf, cbytes) < 1) error;

		crypto_secretstream_xchacha20poly1305_pull(&state, mbuf, NULL, NULL, cbuf, cbytes, NULL, 0);
		if(writeall(STDOUT_FILENO, mbuf, mbytes) < 1) error;
	}
}

int main(int argc, char** argv) {
	if(argc < 2) encrypt();
	else decrypt(argv[1]);
	return 0;
}
