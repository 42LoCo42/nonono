DEPS    := gtk+-3.0
CFLAGS  := $(shell pkg-config --cflags $(DEPS)) -Wall -Wextra
LDFLAGS := $(shell pkg-config --libs   $(DEPS))

all: .ccls main builder
.ccls:
	echo clang > $@
	pkg-config --cflags $(DEPS) \
	| tr ' ' '\n' \
	>> $@
