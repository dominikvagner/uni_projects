RM=rm -f
CC=gcc
CFLAGS=-c -Wall -std=c99 -O3 -pedantic

#create executable file
all: error.o main.o parser.o prec_table.o scanner.o stack.o str.o symtable.o tokenList.o generator.o
	$(CC) error.o main.o parser.o prec_table.o scanner.o stack.o str.o symtable.o tokenList.o generator.o -lm -o ifj

clean:
	$(RM) *.o $(objects) ifj
