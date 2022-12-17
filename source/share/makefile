TARGET  := libjackutils.so

BUILD_DIR    := $(BUILD_DIR)/share
OUTPUT_DIR   ?= .../bin #should be normally defined
SOURCE_DIRS  ?= $(LEVEL_DIR)
INCLUDE_DIR  += $(LEVEL_DIR)

SRCS := $(shell find $(SOURCE_DIRS) -name '*.c')
OBJS := $(SRCS:$(SOURCE_DIRS)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
UNIFLAGS  ?= -O1 -fsanitize=undefined -Wall -Wextra -flto
LINFLAGS  += -shared 
CCFLAGS   ?= -MMD -MP -std=gnu11
CCFLAGS   += $(INC_FLAGS) $(UNIFLAGS) -fPIC

SHRDLIBS  = -lm -ljack -lglfw -lGL

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	@echo "$@ in path $(OUTPUT_DIR)"
	@$(MKDIR_P) $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDLIBS) $(UNIFLAGS) $(LINFLAGS)

# c source
$(BUILD_DIR)/%.o: $(SOURCE_DIRS)/%.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

-include $(DEPS)

MKDIR_P ?= mkdir -p
