#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
	printf(
		"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		strerror(ERANGE)[0],
		strerror(EHWPOISON)[1],
		strerror(EUNATCH)[12],
		strerror(EXDEV)[15],
		strerror(EILSEQ)[9],
		strerror(EBADE)[7],
		strerror(EBADMSG)[9],
		strerror(EHWPOISON)[3],
		strerror(ECONNRESET)[2],
		strerror(ESTALE)[13],
		strerror(EISDIR)[3],
		strerror(ELIBBAD)[9],
		strerror(EADDRNOTAVAIL)[11],
		strerror(ENOKEY)[4],
		strerror(EIDRM)[15],
		strerror(ETXTBSY)[1],
		strerror(EKEYREVOKED)[3],
		strerror(EBUSY)[22],
		strerror(EDQUOT)[7],
		strerror(EBADR)[11],
		strerror(ENOEXEC)[4],
		strerror(E2BIG)[3],
		strerror(EOVERFLOW)[35],
		strerror(EEXIST)[4]
	);
}
