#Output library file
TARGET_EXEC ?= waveform
SRC_DIRS ?= .

SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.cpp')
OBJS += $(SRCS:%=$(BLD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INCFLAGS += $(addprefix -I,$(shell find $(SRC_DIRS) -type d))
CMPFLAGS += $(INCFLAGS) -MMD -MP
CLDFLAGS += 
LDLIBS   += -lGL -lglfw

# target
$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CLDFLAGS) $(OBJS) -o $@ 
# c++
$(BLD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# c source
$(BLD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

-include $(DEPS)

MKDIR_P ?= mkdir -p

