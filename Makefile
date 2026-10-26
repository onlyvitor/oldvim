FLAGS := -Wall -Werror

SRC := main.c io/scan.c io/input.c fileio/file_op.c
TARGET = oldvim

INPUT := text.rst

all:
	gcc $(SRC) -o $(TARGET) $(FLAGS)

run:
	./$(TARGET) $(INPUT)
