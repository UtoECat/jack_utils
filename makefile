# Three level building system
# Copyright (C) UtoECat 2022. All rights Reserved!
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

export

LEVEL_SHR    := $(realpath ./source/share)
LEVEL_DOC    := $(realpath ./doc)
LEVEL_USR    := $(realpath ./source/utils)
LEVEL_DIR    = fatal_error

OUTPUT_DIR   := $(realpath ./bin)
BUILD_DIR    := $(realpath .)/build
INCLUDE_DIR  := $(realpath ./source/include)

UNIFLAGS     ?= -O0 -fsanitize=undefined -Wall -Wextra
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
