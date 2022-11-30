export

SOURCES_RAW_DIRS  = ./jackutils/ ./source/
MAKEFILES = $(shell find $(SOURCES_RAW_DIRS) -name makefile)

#exported variables
BINARY_DIR   ?= $(realpath ./bin)
INCLUDE_DIRS ?= $(realpath ./include)
INCLUDE_LIBS ?= $(BINARY_DIR)/jackutils.o 

#universal flags => for linker and all compilers
UNIFLAGS ?= -O2 -fsanitize=undefined -Wall -Wextra
# LINker flags... (DON'T ADD ./jackutils.o BY DEFAULT!!1)
LINFLAGS ?=

#toremove
OBJFILEST = $(shell find . -name '*.o' -or -name '*.d')

# Not boring ~~wallpapers~~ colors
CReset=\033[0m
# Regular Colors
CRed=\033[0;31m
CGreen=\033[0;32m
CYellow=\033[0;33m
CBlue=\033[0;34m
CCyan=\033[0;36m

.PHONY : all clean $(MAKEFILES) $(OBJFILEST)
all : $(MAKEFILES)
	@echo -e "[Make] $(CGreen)Project is sucessfully maked!$(CReset)"
	@echo -e "[Make] $(CYellow)Makefiles :" $(MAKEFILES) "$(CReset)"

$(MAKEFILES) :
	@echo -e "[Make] $(CYellow) Making $(TARGET) $(CReset)"
	@make -C $(dir $@)
	@echo -e "[MAKE] $(CGreen)Sucess making $(dir $@) $(CReset)"

$(MAKEFILES) : TARGET = $(shell basename $(dir $@))


clean : $(OBJFILES) 
	@echo -e "[Make] $(CGreen) Cleaning up...$(CReset)"
	$(RM) $(OBJFILEST)

$(OBJFILES) :
	@echo $@
