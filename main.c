#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h> 
#include "quickSort.h"
#include "hyperQuickSort.h"


int main(int argc, const char* argv[]) {
 //TODO: citanie pola s file
    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);

    printf("-------------------------------------QUICKSORT-------------------------\n");
    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n");

    clock_t t; 
    t = clock();
    quicksort(0, size - 1, array);
    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
  
    printf("quicksort() took %f seconds to execute \n", time_taken);


    printf("Sorted array: ");
    printArray(array,size);
    printf("\n");
    printf("-------------------------------------QUICKSORT-------------------------\n");

    printf("-------------------------------------PARALLEL QUICKSORT-------------------------\n");
    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n");

    clock_t t2; 
    t2 = clock();
    parallelQuickSort(array,size);
    t2 = clock() - t2; 
    double time_taken2 = ((double)t2)/CLOCKS_PER_SEC; // in seconds 
  
    printf("parallelQuickSort() took %f seconds to execute \n", time_taken2);

    printf("Sorted array: ");
    printArray(array,size);
    printf("\n");
    printf("-------------------------------------PARALLEL QUICKSORT-------------------------\n");

    return 0;
    
}