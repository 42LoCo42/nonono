CFLAGS := -Wall -Wextra $(shell pkg-config --cflags fluidsynth)
LDFLAGS := $(shell pkg-config --libs fluidsynth)

all: playtest metronome
%: %.c
