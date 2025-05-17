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

TARGETS := main.efi main.img main.iso

all: $(TARGETS)

##### main.efi #################################################################

%.efi: %.so
	$(OBJCOPY) $(OCFLAGS) $< $@

%.so: %.o
	$(LD) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

##### main.img #################################################################

main.img: main.efi
	truncate -s 48M $@
	mformat -i $@ -F
	mmd     -i $@ ::/EFI
	mmd     -i $@ ::/EFI/BOOT
	mcopy   -i $@ $< ::/EFI/BOOT/BOOTX64.EFI

##### main.iso #################################################################

main.iso: main.img
	rm -rfv  iso
	mkdir -p iso
	cp $<    iso
	xorriso -as mkisofs -R -f -e $< -no-emul-boot -o $@ iso

################################################################################

install: $(TARGETS)
	install $(TARGETS) $(out)
.PHONY: install

clean:
	rm -fv *.efi *.so *.o
.PHONY: clean

run: main.efi
	# create root directory
	rm -rfv     root
	mkdir -p    root/EFI/BOOT
	cp main.efi root/EFI/BOOT/BOOTX64.EFI
	echo abc | iconv -t utf16le > root/data

	# create local OVMF image
	install -m644 $(OVMF) OVMF.fd

	# start QEMU
	qemu-system-x86_64                           \
		-enable-kvm                              \
		-drive if=pflash,format=raw,file=OVMF.fd \
		-drive format=raw,file=fat:rw:root       \
		-net none
.PHONY: run
