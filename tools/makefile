TARGET ?= img2header
CC ?= gcc

$(TARGET) : img2header.c
	$(CC) $< -o $@ -lm -flto -I./
