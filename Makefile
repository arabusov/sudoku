# MIT License
#
# Copyright (c) 2023 Andrei Rabusov

CC=cc
CFLAGS=-ansi -pedantic -Wall -Wextra -Werror

all: sudoku

sudoku: sudoku.c
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "  [CC]	$@"
test:
	@bash test.sh
clean:
	@rm -f sudoku
	@echo "  [RM]	sudoku"

PHONY: all, sudoku, clean, test
