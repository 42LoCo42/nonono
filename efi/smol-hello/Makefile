CFLAGS :=                      \
	-fpic                      \
	-ffreestanding             \
	-fno-stack-protector       \
	-fno-stack-check           \
	-fshort-wchar              \
	-mno-red-zone              \
	-maccumulate-outgoing-args \

LDFLAGS :=                               \
	-shared                              \
	-Bsymbolic                           \
	-T $(GNU_EFI)/lib/elf_x86_64_efi.lds \
	$(GNU_EFI)/lib/crt0-efi-x86_64.o     \
	-lgnuefi                             \
	-lefi                                \

OCFLAGS :=                  \
	-j .text                \
	-j .sdata               \
	-j .data                \
	-j .rodata              \
	-j .dynamic             \
	-j .dynsym              \
	-j .rel                 \
	-j .rela                \
	-j .rel.*               \
	-j .rela.*              \
	-j .reloc               \
	--target efi-app-x86_64 \
	--subsystem=10          \

all: main.efi

%.efi: %.so
	$(OBJCOPY) $(OCFLAGS) $< $@

%.so: %.o
	$(LD) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: main.efi
	install -Dm644 $< $(out)/$<
.PHONY: install

clean:
	rm -fv *.efi *.so *.o

run: main.efi
	# create root directory
	mkdir -p    root/EFI/BOOT
	cp main.efi root/EFI/BOOT/BOOTX64.EFI
	echo abc | iconv -t utf16le > root/data

	# create local OVMF image
	cp $(OVMF) OVMF.fd
	chmod +w OVMF.fd

	# start QEMU
	qemu-system-x86_64                           \
		-enable-kvm                              \
		-drive if=pflash,format=raw,file=OVMF.fd \
		-drive format=raw,file=fat:rw:root       \
		-net none
.PHONY: run
