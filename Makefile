CC=gcc
CFLAGS=-std=c11 -I -lpthread
OUTPUT=sort

all: $(OUTPUT)

$(OUTPUT): functions.o quickSort_p_8.o quickSort_p_4.o quickSort_p_2.o quickSort.o main.o
	$(CC) $(CFLAGS) quickSort_p_8.o quickSort_p_4.o quickSort_p_2.o quickSort.o functions.o main.o  -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c  -o main.o

quickSort_p_8.o: quickSort_p_8.c quickSort.h quickSort_p_8.h functions.h functions.c
	$(CC) $(CFLAGS) -c quickSort_p_8.c   -o quickSort_p_8.o 

quickSort_p_4.o: quickSort_p_4.c quickSort.h quickSort_p_4.h functions.h functions.c
	$(CC) $(CFLAGS) -c quickSort_p_4.c  -o quickSort_p_4.o 

quickSort_p_2.o: quickSort_p_2.c quickSort.h quickSort_p_2.h functions.h functions.c
	$(CC) $(CFLAGS) -c quickSort_p_2.c  -o quickSort_p_2.o  
	
functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c -o functions.o

quickSort.o: quickSort.c quickSort.h
	$(CC) $(CFLAGS) -c quickSort.c  -o quickSort.o
