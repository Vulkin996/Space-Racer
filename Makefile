PROGRAM = space_racer
CC      = gcc
CFLAGS  = -Wall
LDLIBS  = -lglut -lGLU -lGL -lm

$(PROGRAM): main.o vector.o spaceship.o vector.h spaceship.h
	$(CC) -o $(PROGRAM) main.o vector.o spaceship.o $(LDLIBS)

main.o: main.c vector.h
	$(CC) -c main.c
vector.o: vector.c vector.h
	$(CC) -c vector.c
spaceship.o: spaceship.c vector.h spaceship.h
	$(CC) -c spaceship.c

.PHONY: clean

clean:
	rm *.o $(PROGRAM)