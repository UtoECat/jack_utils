# Quick and Dirty makefile to run other makefiles in source dir :D
export
#directories
CURR_DIR = $(shell pwd)
BLD_DIR  = $(CURR_DIR)/build   # build directory
BIN_DIR  = $(CURR_DIR)/bin     # output binaries
INC_DIR  = $(CURR_DIR)/include # shared headers

TARGETS := $(shell find ./source -name 'makefile')

LDLIBS   := -lm -ljack
INCFLAGS := -I$(INC_DIR)
CMPFLAGS ?= -O0 -fsanitize=address -Wall -Wextra
CCFLAGS  := -std=gnu11 
CXXFLAGS := -fno-exceptions -std=c++20
CLDFLAGS := 

.PHONY: $(TARGETS) clear help all libs tools

help :
	echo "help  - show this help"
	echo "all   - make anything (default)"
	echo "libs  - make shared libraries  "
	echo "tools - make tools"
	echo "install - unused"

all : libs tools

libs :
	$(MAKE) -C ./jackutils

tools : $(TARGETS)

$(TARGETS) :
	$(MAKE) -C $(dir $@)

clear :
	$(RM) -rf $(BLD_DIR)
