all: src/minesweeper.c
	gcc src/minesweeper.c -lncurses -Wall -o minesweeper
