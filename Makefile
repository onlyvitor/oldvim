FLAGS := -Wall -Werror

SRC := main.c scan.c scan.h
TARGET = oldvim

INPUT := text.rst

all:
	gcc $(SRC) -o $(TARGET) $(FLAGS)

run:
	./$(TARGET) $(INPUT)
