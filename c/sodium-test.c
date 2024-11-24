#include <err.h>
#include <sodium.h>
#include <stdio.h>
#include <string.h>

#define check(c, ...) if(c) err(1, __VA_ARGS__);

int main() {
	check(sodium_init() < 0, "Could not init libsodium");

	unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES] = {0};
	crypto_secretstream_xchacha20poly1305_keygen(key);

	unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES] = {0};

	crypto_secretstream_xchacha20poly1305_state send_state = {0};
	check(crypto_secretstream_xchacha20poly1305_init_push(
		&send_state, header, key) < 0, "Could not init send state");

	crypto_secretstream_xchacha20poly1305_state recv_state = {0};
	check(crypto_secretstream_xchacha20poly1305_init_pull(
		&recv_state, header, key) < 0, "Could not init recv state");

	const char*  msg = "hello";
	const size_t mlen = strlen(msg);

	size_t clen = mlen + crypto_secretstream_xchacha20poly1305_ABYTES;
	char*  enc  = malloc(clen);
	char*  dec  = malloc(mlen);

	for(int i = 0; i < 5; i++) {
		check(crypto_secretstream_xchacha20poly1305_push(
			&send_state,
			enc, NULL, msg, mlen, NULL, 0,
			// crypto_secretstream_xchacha20poly1305_TAG_REKEY
			0
			) < 0, "Could not encrypt");

		for(size_t i = 0; i < clen; i++) {
			printf("%d ", enc[i]);
		}
		puts("");

		check(crypto_secretstream_xchacha20poly1305_pull(
			&recv_state,
			dec, NULL, NULL, enc, clen, NULL, 0
		) < 0, "Could not decrypt");
	}
}
