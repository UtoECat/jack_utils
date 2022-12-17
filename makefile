# Three level building system
# Copyright (C) 2022 UtoECat
# GNU GPL License. No any warrianty

export

LEVEL_SHR    := $(realpath ./source/share)
LEVEL_DOC    := $(realpath ./doc)
LEVEL_USR    := $(realpath ./source/utils)
LEVEL_DIR    = fatal_error

OUTPUT_DIR   := $(realpath ./bin)
BUILD_DIR    := $(realpath .)/build
INCLUDE_DIR  := $(realpath ./source/include)

UNIFLAGS     ?= -O2 -fsanitize=undefined -fsanitize=address -Wall -Wextra
CCFLAGS      ?= -std=gnu17 -MMD -MP

CNUL = \033[0m
CERR = \033[0;31m
CSUC = \033[0;32m
CPRC = \033[0;33m

.PHONY: info all doc usr share dirs clear clean run

info:
	@echo -en "$(CPRC)"
	@echo "call with argument all to build anything :)"
	@echo "or with doc or usr or share to build something one :)"
	@echo -en "$(CNUL)"

dirs:
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(BUILD_DIR)

all: usr doc

usr : LEVEL_DIR = $(LEVEL_USR)
usr : dirs share
	@echo -en "$(CPRC)[Make] Making level $(LEVEL_DIR) $(CNUL)\n"
	@$(MAKE) -f $(LEVEL_DIR)/makefile

share: LEVEL_DIR = $(LEVEL_SHR)
share: dirs
	@echo -en "$(CPRC)[Make] Making level $(LEVEL_DIR) $(CNUL)\n"
	@$(MAKE) -f $(LEVEL_DIR)/makefile

doc: LEVEL_DIR = $(LEVEL_DOC)
doc:
	@echo -en "$(CPRC)[Make] Making level $(LEVEL_DIR) $(CNUL)\n"
	@$(MAKE) -f $(LEVEL_DIR)/makefile

clear: clean
clean:
	rm -rf $(BUILD_DIR)
