CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: assembler

assembler: assembler.o demacro.o phaseOne.o phaseTwo.o fileMaker.o util.o tables.o
	$(CC) $(CFLAGS) -o assembler assembler.o demacro.o phaseOne.o phaseTwo.o fileMaker.o util.o tables.o

assembler.o: assembler.c assembler.h header.h
	$(CC) $(CFLAGS) -c assembler.c

demacro.o: demacro.c demacro.h header.h
	$(CC) $(CFLAGS) -c demacro.c

phaseOne.o: phaseOne.c phaseOne.h tables.h header.h
	$(CC) $(CFLAGS) -c phaseOne.c

phaseTwo.o: phaseTwo.c phaseTwo.h tables.h header.h
	$(CC) $(CFLAGS) -c phaseTwo.c

fileMaker.o: fileMaker.c fileMaker.h tables.h header.h
	$(CC) $(CFLAGS) -c fileMaker.c

util.o: util.c util.h header.h
	$(CC) $(CFLAGS) -c util.c

tables: tables.c tables.h header.h
	$(CC) $(CFLAGS) -c tables.c

clean:
	rm -fv assembler *.o *.am *.ob *.ent *.ext