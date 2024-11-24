DEPS    := glfw3
CFLAGS  := $(shell pkg-config --cflags $(DEPS)) -Iinclude -Wall -Wextra -g
LDFLAGS := $(shell pkg-config --libs   $(DEPS)) -lGL

main: \
	main.c \
	src/glad.c \
	gen/vertexShaderSrc.glsl.h \
	gen/fragmentShaderSrc.glsl.h

gen/%.h: src/%
	./fileToC.sh $< > $@
