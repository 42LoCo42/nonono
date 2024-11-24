#include <stdio.h>

#include "cert"
#include "key"

int main() {
	FILE* certf = fopen("cert.bin", "w");
	fwrite(cert, cert_len, 1, certf);

	FILE* keyf = fopen("key.bin", "w");
	fwrite(key, key_len, 1, keyf);
}
