TARGET  ?= unnamed

BUILD_DIR    ?= ./build
BINARY_DIR   ?= ../bin #should be normally defined
SOURCE_DIRS  ?= ./
#INCLUDE_DIRS ?= ../ #too
INCLUDE_DIRS += ./
INCLUDE_LIBS ?= $(BINARY_DIR)/jackutils.o #it too, but just a atest

SRCS := $(shell find $(SOURCE_DIRS) -name '*.c' -or -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))
UNIFLAGS ?= -O0 -fsanitize=undefined -Wall -Wextra
LINFLAGS += $(INCLUDE_LIBS) 
CMPFLAGS ?= $(INC_FLAGS) -MMD -MP $(UNIFLAGS) -std=gnu11 
CXXFLAGS ?= $(INC_FLAGS) -MMD -MP $(UNIFLAGS) -std=gnu++20 -fno-exceptions

SHRDLIBS  = -lm -ljack -lglfw -lGL

$(BINARY_DIR)/$(TARGET): $(OBJS)
	@echo "$@ in path $(BINARY_DIR)"
	$(MKDIR_P) $(dir $@)
	$(CXX) $(OBJS) -o $@ $(SHRDLIBS) $(UNIFLAGS) $(LINFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CMPFLAGS) -c $< -o $@

# cpp source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@



.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
