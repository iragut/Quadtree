# Declaratiile de variabile
CC = gcc
CFLAGS = -g -Wall -lm -std=c99
 
build: Tema2.c
	$(CC) -o quadtree Quadtree.c $(CFLAGS)

run:
	valgrind ./tema1
.PHONY : clean
clean :
	rm -f tema1
	rm -f *.out
	rm my_diff
