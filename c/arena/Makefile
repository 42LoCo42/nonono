OUTPUT   := arena
DEPS     :=
FLAGS    := -Wall -Wextra -Iinc
CFLAGS   :=
CXXFLAGS :=
LDFLAGS  :=

# don't change things beyond this line

SOURCES  := $(wildcard src/*.c) $(wildcard src/*.cpp)
OBJECTS  := $(patsubst src/%.cpp,out/%.o,$(patsubst src/%.c,out/%.o,$(SOURCES)))

ifneq ($(strip $(DEPS)),)
PKG_FLAGS   := $(shell pkg-config --cflags $(DEPS))
PKG_LDFLAGS := $(shell pkg-config --libs   $(DEPS))
endif

all: $(OUTPUT)

out:
	mkdir -p $@

$(OUTPUT): out $(OBJECTS)
	$(CC) \
		$(PKG_LDFLAGS) \
		$(LDFLAGS) \
		$(filter-out out,$^) \
		-o $@

out/%.o: src/%.c
	$(CC) \
		$(PKG_FLAGS) \
		$(FLAGS) \
		$(CFLAGS) \
		-c $< -o $@

out/%.o: src/%.cpp
	$(CXX) \
		$(PKG_FLAGS) \
		$(FLAGS) \
		$(CXXFLAGS) \
		-c $< -o $@

install: $(OUTPUT)
	install -Dm755 $< $(out)/bin/$<

clean:
	rm -rf out $(OUTPUT)
.PHONY: clean
