CC := clang
CFLAGS := -I. -std=c17 -Wall -Wextra -Werror
DEBUG_FLAGS := -g
RELEASE_FLAGS := -O2
SANITIZE_FLAGS := -g -fsanitize=address,undefined

SRC := $(wildcard *.c) $(wildcard io/*.c) $(wildcard fileio/*.c)
HEADERS := $(wildcard *.h) $(wildcard io/*.h) $(wildcard fileio/*.h)
TARGET := oldvim

INPUT := text.rst

.PHONY: all clean run test test-sanitize debug release

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SRC) -o $@

debug: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(TARGET)

release: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT)

test: $(TARGET)
	python3 tests/test_editor.py

test-sanitize:
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) $(SRC) -o $(TARGET)
	python3 tests/test_editor.py

clean:
	rm -f $(TARGET)
