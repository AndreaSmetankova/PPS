#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <unistd.h>
#include <time.h> 
#include "quickSort.h"
#include "hyperQuickSort.h"
void generateNumFile(int size,char* file);
void generateNumArray(int size, int* array);


int main(int argc, const char* argv[]) {

    // int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int size = sizeof(array) / sizeof(array[0]);
    // int array2[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    // int size2 = sizeof(array2) / sizeof(array2[0]);
    // generateNumFile(1000,"numbers.txt");
    int size = 10000;
    int array[size];
    int array2[size];
    generateNumArray(size, array);
    copyArray(array, array2, size, 0);
    

    printf("-------------------------------------QUICKSORT-------------------------\n");
    printf("Unsorted array: ");
    // printArray(array,size);
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
    printf("-------------------------------------QUICKSORT-------------------------\n");

    printf("-------------------------------------PARALLEL QUICKSORT-------------------------\n");
    printf("Unsorted array: ");
    // printArray(array2,size);
    printf("\n");

    clock_t t2; 
    t2 = clock();
    parallelQuickSort(array2,size);
    t2 = clock() - t2; 
    double time_taken2 = ((double)t2)/CLOCKS_PER_SEC; // in seconds 
  
    printf("parallelQuickSort() took %f seconds to execute \n", time_taken2);

    printf("Sorted array: ");
    // printArray(array2,size);
    printf("\n");
    printf("-------------------------------------PARALLEL QUICKSORT-------------------------\n");

    return 0;
    
}

void generateNumFile(int size,char* file){
    
    FILE* fp;
    fp = fopen(file, "w+");

    srand(time(0));
 
    for(int i = 0; i < size; i++)
    {
        if(i == (size - 1))
        {
            fprintf(fp,"%d",rand() ); 
        } else
        {
            fprintf(fp,"%d,",rand());
        }
        
    }
        
    fclose(fp);

}

void generateNumArray(int size,int* result){
    srand(time(0));

    for(int i = 0; i < size; i++)
    {
     result[i] = rand();   
    }


}