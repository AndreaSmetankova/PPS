#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include "quickSort.h"
#include "quickSort_p_2.h"
#include "quickSort_p_4.h"
#include "quickSort_p_8.h"
#include "functions.h"
void generateNumArray(int size, int* array);


int main(int argc, const char* argv[]) {

    // int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int array2[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int array3[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int array4[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int size = sizeof(array) / sizeof(array[0]);
    // int size2 = sizeof(array2) / sizeof(array2[0]);
    int size = 100000;
    int array[size];
    int array2[size];
    int array3[size];
    int array4[size];
    generateNumArray(size, array);
    copyArray(array, array2, size, 0);
    copyArray(array, array3, size, 0);
    copyArray(array, array4, size, 0);
    
    printf("ARRAY SIZE = %i \n", size);
    printf("Unsorted array: ");
    // printArray(array,size);

    printf("-------------------------------------QUICKSORT-------------------------\n");
    
    printf("\n");

    clock_t t; 
    t = clock();
    quicksort(0, size - 1, array);
    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
  
    printf("quicksort() took %f seconds to execute \n", time_taken);


    printf("Sorted array: ");
    // printArray(array,size);
    printf("\n");
    printf("---------------------------------------------------------------------\n");


    printf("\n-------------------------------------PARALLEL 2 threads QUICKSORT-------------------------\n");
    clock_t t2; 
    t2 = clock();
    parallelQuickSort_2threads(array2,size);
    t2 = clock() - t2; 
    double time_taken2 = ((double)t2)/CLOCKS_PER_SEC; // in seconds 
  
    printf("parallelQuickSort() with 2 threads took %f seconds to execute \n", time_taken2);

    printf("Sorted array: ");
    // printArray(array2,size);
    printf("\n");
    printf("-----------------------------------------------------------------------------------------------\n");

    printf("\n-------------------------------------PARALLEL 4 threads QUICKSORT-------------------------\n");
    clock_t t3; 
    t3 = clock();
    parallelQuickSort_4threads(array3,size);
    t3 = clock() - t3; 
    double time_taken3 = ((double)t3)/CLOCKS_PER_SEC; // in seconds 
  
    printf("parallelQuickSort() with 4 threads took %f seconds to execute \n", time_taken3);

    printf("Sorted array: ");
    // printArray(array3,size);
    printf("\n");
    printf("---------------------------------------------------------------------------------------------\n");

    printf("\n-------------------------------------PARALLEL 8 threads QUICKSORT-------------------------\n");
    clock_t t4; 
    t4 = clock();
    parallelQuickSort_8threads(array4,size);
    t4 = clock() - t4; 
    double time_taken4 = ((double)t4)/CLOCKS_PER_SEC; // in seconds 
  
    printf("parallelQuickSort() with 8 threads took %f seconds to execute \n", time_taken4);

    printf("Sorted array: ");
    // printArray(array4,size);
    printf("\n");
    printf("------------------------------------------------------------------------------------------------\n");

    return 0;
    
}


void generateNumArray(int size,int* result){
    srand(time(0));

    for(int i = 0; i < size; i++)
    {
     result[i] = rand();   
    }


}