CC=gcc
CFLAGS=-std=c11 -I -lpthread
OUTPUT=sort

all: $(OUTPUT)

$(OUTPUT): hyperQuickSort.o quickSort.o main.o
	$(CC) $(CFLAGS) hyperQuickSort.o quickSort.o main.o  -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c  -o main.o

hyperQuickSort.o: hyperQuickSort.c quickSort.h hyperQuickSort.h
	$(CC) $(CFLAGS) -c hyperQuickSort.c  -o hyperQuickSort.o 

quickSort.o: quickSort.c quickSort.h
	$(CC) $(CFLAGS) -c quickSort.c  -o quickSort.o
