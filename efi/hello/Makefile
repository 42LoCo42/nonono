ARCH := x86_64
CC   := x86_64-w64-mingw32-gcc

CFLAGS :=                      \
	-Wall -Wextra              \
	-D HAVE_USE_MS_ABI         \
	-flto                      \
	-I $(GNU_EFI)/inc          \
	-I $(GNU_EFI)/inc/x86_64   \
	-I $(GNU_EFI)/inc/protocol \

LDFLAGS :=             \
	-shared            \
	-Wl,-dll           \
	-Wl,--subsystem,10 \
	-e efi_main        \

SRCS :=                                    \
	$(wildcard *.c)                        \
	$(wildcard $(GNU_EFI)/lib/*.c)         \
	$(wildcard $(GNU_EFI)/lib/runtime/*.c) \
	$(wildcard $(GNU_EFI)/lib/$(ARCH)/*.c) \
	$(wildcard $(GNU_EFI)/lib/$(ARCH)/*.S) \

OBJS := $(addprefix out/, $(patsubst %.S, %.o, $(patsubst %.c, %.o, $(subst $(GNU_EFI)/,, $(SRCS)))))
DIRS := $(sort $(dir $(OBJS)))

all: dirs out/main.efi

dirs:
	mkdir -p $(DIRS)

out/main.efi: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

out/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: $(GNU_EFI)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: $(GNU_EFI)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	# create root directory
	mkdir -p        root/EFI/BOOT
	cp out/main.efi root/EFI/BOOT/BOOTX64.EFI
	echo abc | iconv -t utf16le > root/data
	# create local OVMF image
	cp $(OVMF)/FV/OVMF.fd out/OVMF.fd
	chmod +w out/OVMF.fd
	# start QEMU
	qemu-system-x86_64                               \
		-enable-kvm                                  \
		-drive if=pflash,format=raw,file=out/OVMF.fd \
		-drive format=raw,file=fat:rw:root           \
		-net none
.PHONY: run

install: all
	mkdir -p "$$out"
	cp out/main.efi "$$out"

clean:
	rm -rf out root
.PHONY: clean
