CFLAGS  := -I /usr/include/efi -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args
LDFLAGS := -shared -Bsymbolic -T /usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o
OCFLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10

.PHONY: run
run: main.efi
	uefi-run $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.so: %.o
	$(LD) $(LDFLAGS) $< -o $@

%.efi: %.so
	objcopy $(OCFLAGS) $< $@
