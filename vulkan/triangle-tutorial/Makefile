NAME    := triangle
DEPS    := fmt glfw3 glm vulkan
CFLAGS  := $(CFLAGS)  -Wall -Wextra -std=c++20
LDFLAGS := $(LDFLAGS) -ldl -lpthread

SHADERS := triangle.frag triangle.vert

################################################################################

ifdef DEPS
CFLAGS  += $(shell pkg-config --cflags $(DEPS))
LDFLAGS += $(shell pkg-config --libs   $(DEPS))
endif

ifdef DEBUG
CFLAGS  += -DDEBUG
endif

SRCD := src
BLDD := build

SRCS := $(patsubst $(SRCD)/%,%,$(shell find $(SRCD) -name '*.cpp'))
INCS := $(patsubst %.cpp,$(BLDD)/%.d,$(SRCS))
OBJS := $(patsubst %.cpp,$(BLDD)/%.o,$(SRCS))
DIRS := $(sort $(shell dirname $(OBJS)))

SHADER-OBJS := $(patsubst %,$(BLDD)/%.spv.o,$(SHADERS))

$(shell mkdir -p $(DIRS))

all: $(NAME)

-include $(INCS)

$(BLDD)/%.spv: $(SRCD)/%
	glslc $< -o $@

$(BLDD)/%.spv.cpp: $(BLDD)/%.spv
	xxd -i $< > $@

$(BLDD)/%.o: $(SRCD)/%.cpp
	$(CXX) $(CFLAGS) -c $< -MMD -o $@

$(NAME): $(OBJS) $(SHADER-OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

run: $(NAME)
	./$<
.PHONY: run

install: $(NAME)
	mkdir -p $(out)/bin
	cp $< $(out)/bin
.PHONY: install

clean:
	rm -rf $(BLDD) $(NAME)
.PHONY: clean
