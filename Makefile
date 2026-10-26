CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Werror -g

SRC := main.c io/scan.c io/input.c fileio/file_op.c
TARGET = oldvim

INPUT := text.rst

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET) $(INPUT)
