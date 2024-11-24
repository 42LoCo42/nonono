#include "sodium/randombytes.h"
#include <err.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define die(...) err(1, __VA_ARGS__);
#define hexprint(name, bin, bin_len)                                           \
	{                                                                          \
		size_t len = bin_len * 2 + 1;                                          \
		char*  hex = malloc(len);                                              \
		printf("%s: %s\n", name, sodium_bin2hex(hex, len, bin, bin_len));      \
		free(hex);                                                             \
	}

static unsigned char seed[randombytes_SEEDBYTES];

ssize_t getrandom(void* buf, size_t buflen, unsigned int flags) {
	(void) flags;
	printf("[determssh] Generating %zu random bytes...\n", buflen);
	randombytes_buf_deterministic(buf, buflen, seed);
	randombytes_buf_deterministic(seed, sizeof(seed), seed);
	return buflen;
}

static void __attribute__((constructor)) setup(void) {
	puts("[determssh] Loading...");

	const char* username = "leonsch";
	const char* password = "password";
	const char* hostname = "astylos";

	unsigned char salt[crypto_pwhash_SALTBYTES] = {0};

	crypto_generichash_state state = {0};

	if(crypto_generichash_init(&state, NULL, 0, sizeof(salt)) < 0)
		die("hash init");

	if(crypto_generichash_update(
		   &state, (unsigned char*) username, strlen(username)
	   ) < 0)
		die("hash update username");

	if(crypto_generichash_update(
		   &state, (unsigned char*) hostname, strlen(hostname)
	   ) < 0)
		die("hash update hostname");

	if(crypto_generichash_final(&state, salt, sizeof(salt)) < 0)
		die("hash final");

	hexprint("[determssh] salt", salt, sizeof(salt));

	if(crypto_pwhash(
		   seed, sizeof(seed), password, strlen(password), salt,
		   crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
		   crypto_pwhash_ALG_DEFAULT
	   ) < 0)
		die("pwhash");

	hexprint("[determssh] seed", seed, sizeof(seed));
}
