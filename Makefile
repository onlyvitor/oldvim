CC := clang
CFLAGS := -I. -std=c17 -Wall -Wextra -Werror -g

SRC := main.c editor.c io/scan.c io/input.c io/screen.c fileio/file_op.c
HEADERS := editor.h io/scan.h io/screen.h fileio/fileio.h
TARGET := oldvim

INPUT := text.rst

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT)

clean:
	rm -f $(TARGET)
