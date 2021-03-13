#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "quickSort.h"
#include "hyperQuickSort.h"


int main(int argc, const char* argv[]) {

    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);
    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n"); 
    quicksort(0,size -1, array);
    printf("Sorted array: ");
    printArray(array,size);
    printf("\n");
    
    
    
    
    parallelQuickSort(array,size);
    

    return 0;
}