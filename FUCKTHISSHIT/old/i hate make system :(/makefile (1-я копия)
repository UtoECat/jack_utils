include (MAKE_DEP)
TARGET_EXEC ?= .o
SRC_DIRS ?= .

CMPFLAGS += $(INCFLAGS) -MMD -MP
CLDFLAGS += -fPIC -r
LDLIBS   += -lGL -lglfw
OBJS := $(SRCS:%=$(BLD_DIR)/%.o) #don't include ourself


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

