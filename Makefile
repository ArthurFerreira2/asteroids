LIBS = -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf
FLAGS = -std=c99 -pedantic -Wpedantic -Wall -Werror -O3 -Wl,-subsystem,windows
CC = gcc

asteroids: asteroids.c
	$(CC) $^ $(FLAGS) $(LIBS) -o $@

all: asteroids
