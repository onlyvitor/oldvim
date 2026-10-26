CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Werror -g

SRC := main.c io/scan.c io/input.c fileio/file_op.c
TARGET := oldvim

INPUT := text.rst

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT)

clean:
	rm -f $(TARGET)
