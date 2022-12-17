export

OLDBUILD    := $(BUILD_DIR)
BUILD_DIR    = $(OLDBUILD)/$(LEVEL_DIR:$(abspath ./source)/%=%)
OUTPUT_DIR   ?= .../bin #should be normally defined
SOURCE_DIRS  = $(LEVEL_DIR)

TARGET_DIR := $(dir $(shell find $(SOURCE_DIRS) -mindepth 2 -type f -iname makefile))

.PHONY: all $(TARGET_DIR)

all: $(TARGET_DIR)

$(TARGET_DIR): LEVEL_DIR = $(abspath $@)
$(TARGET_DIR):
	cd $@ && make
