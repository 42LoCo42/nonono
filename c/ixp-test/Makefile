CFLAGS := -Wall -Wextra

%: %.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

all: main alwaysReuseAddr

main: LDFLAGS += -lixp
main: main.c other.c

alwaysReuseAddr: CFLAGS  += -fPIC -shared
alwaysReuseAddr: LDFLAGS += -ldl
