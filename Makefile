CC := clang
CFLAGS := -std=c17 -Wall -Wextra -Werror -g

SRC := main.c io/scan.c io/input.c io/screen.c fileio/file_op.c helpers/main_helper.c
HEADERS := io/scan.h fileio/fileio.h helpers/helper.h
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
