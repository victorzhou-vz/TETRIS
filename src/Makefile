# Contenu du Makefile

# Exemple de règles de compilation
CC=gcc
CFLAGS=-I. 

all: tetris

tetris: main.o game.o
	$(CC) -o tetris main.o game.o

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

game.o: game.c
	$(CC) -c game.c $(CFLAGS)

clean:
	rm -f *.o tetris