TARGET  ?= jackutils.o
BIN_DIR = ./../bin
SRC_DIR ?= .
BLD_DIR = ./build
#BLD_DIR ?= $(shell echo $(BLD_DIR)/jackutils)

SRCS ?= $(shell find $(SRC_DIR) -name '*.c' -or -name '*.cpp')
OBJS ?= $(SRCS:%=$(BLD_DIR)/%.o)
DEPS ?= $(OBJS:.o=.d)

INCFLAGS += $(addprefix -I,$(shell find $(SRC_DIR) -type d))
CMPFLAGS += $(INCFLAGS) -MMD -MP

CCFLAGS  += $(CMPFLAGS)
CXXFLAGS += $(CMPFLAGS) 

LDLIBS   += -lGL -lglfw
CLDFLAGS += $(CMPFLAGS) $(LDLIBS) -fPIC -r

# target
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(OBJS) -o $@ $(CLDFLAGS)
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

