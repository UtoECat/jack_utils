TARGET ?= waveform

OUTPUT_DIR ?= ./bin #must be normally defined
SOURCE_DIRS ?= ./
INCLUDE_DIR += $(LEVEL_DIR)

SRCS := $(shell find $(SOURCE_DIRS) -name '*.c')
OBJS := $(SRCS:$(SOURCE_DIRS)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
UNIFLAGS ?= -O0 -fsanitize=undefined -Wall -Wextra
LINFLAGS += -L$(OUTPUT_DIR) $(SHRDLIBS)
CCFLAGS  ?= -MMD -MP -std=gnu11 
CCFLAGS  += $(INC_FLAGS) $(UNIFLAGS)

SHRDLIBS  = -lm -ljack -lglfw -lGL -ljackutils

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	echo "[MAKE] $(OUTPUT_DIR)"
	@$(MKDIR_P) $(dir $@)
	$(CC) $< -o $@ $(UNIFLAGS) $(LINFLAGS)

# c source
$(BUILD_DIR)/%.o: $(SOURCE_DIRS)/%.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

#-include $(DEPS)

MKDIR_P ?= mkdir -p
