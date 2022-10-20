# Quick and Dirty makefile to run other makefiles in source dir :D

export # export everything :)

ROOT_DIR = $(shell pwd) # root make directory
MAKE_DEP = $(CURR_DIR)/make_magic # stuff included in every makefile

BLD_DIR ?= $(CURR_DIR)/build   # build directory
BIN_DIR ?= $(CURR_DIR)/bin     # output binaries
INC_DIR ?= $(CURR_DIR)/include # shared headers
TAR_DIR ?= $(CURR_DIR)/source  # directory with targets

TARGETS := $(shell find $(TAR_DIR) -name 'makefile')
LDLIBS   = -lm -ljack

INCFLAGS = -I$(INC_DIR)
CMPFLAGS = -O0 -fsanitize=address -Wall -Wextra
CCFLAGS  = -std=c11 
CXXFLAGS = -fno-exceptions -std=c++20
CLDFLAGS = 

all : $(TARGETS)
.PHONY: all $(TARGETS)

$(TARGETS) : $(BIN_DIR)/$@
	$(MAKE) -C $@

$(TARGETS) : TARGET = $@

.PHONY: clear

clear :
	$(RM) -rf $(BLD_DIR)
