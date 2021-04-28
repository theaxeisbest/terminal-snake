debug:
	gcc -g terminal-snake.c -o terminal-snake -lpthread

all:
	gcc -O3 terminal-snake.c -o terminal-snake -lpthread