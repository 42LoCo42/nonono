NAME    := triangle
DEPS    := glfw3 glm vulkan
CFLAGS  := $(CFLAGS)  -Wall -Wextra -std=c++17
LDFLAGS := $(LDFLAGS) -ldl -lpthread

################################################################################

ifdef DEPS
CFLAGS  += $(shell pkg-config --cflags $(DEPS))
LDFLAGS += $(shell pkg-config --libs   $(DEPS))
endif

SRCD := src
BLDD := build

SRCS := $(patsubst $(SRCD)/%,%,$(shell find $(SRCD) -name '*.cpp'))
INCS := $(patsubst %.cpp,$(BLDD)/%.d,$(SRCS))
OBJS := $(patsubst %.cpp,$(BLDD)/%.o,$(SRCS))
DIRS := $(sort $(shell dirname $(OBJS)))

$(shell mkdir -p $(DIRS))

all: $(NAME)

-include $(INCS)

$(BLDD)/%.o: $(SRCD)/%.cpp
	$(CXX) $(CFLAGS) -c $< -MMD -o $@

$(NAME): $(OBJS)
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
