CC=gcc
ASM=nasm
CFLAGS=-c -Wall
ASFLAGS=-f elf64

all: hello

hello: main.c my_print.o
	$(CC) -o hello main.c my_print.o

my_print.o: my_print.asm
	$(ASM) $(ASFLAGS) -o my_print.o my_print.asm

clean:
	rm -f *.o hello
