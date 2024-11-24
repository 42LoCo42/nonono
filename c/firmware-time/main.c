#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "types.h"

#define check(cond, ...)                                \
	if(cond) {                                          \
		fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
		err(1, __VA_ARGS__);                            \
	}                                                   \

int main() {
	struct acpi_table_header tbl;
	struct acpi_fpdt_header  hdr;
	struct acpi_fpdt_boot_header hbrec;
	struct acpi_fpdt_boot brec;

	int fd = open("/sys/firmware/acpi/tables/FPDT", O_RDONLY);
	check(fd < 0, "Could not open FPDT table");

	ssize_t len = read(fd, &tbl, sizeof(tbl));
	check(len != sizeof(tbl), "Invalid table header");

	len = read(fd, &hdr, sizeof(hdr));
	check(len != sizeof(hdr), "Invalid table entry");

	close(fd);
	fd = open("/dev/mem", O_RDONLY);
	check(fd < 0, "Could not open memory");

	len = pread(fd, &hbrec, sizeof(hbrec), hdr.ptr);
	check(len != sizeof(hbrec), "Invalid record header");

	len = pread(fd, &brec, sizeof(brec), hdr.ptr + sizeof(hbrec));
	check(len != sizeof(brec), "Invalid record");

	printf(
		"Firmware load time: %fs\n",
		brec.exit_services_exit / 1e9
	);
}
