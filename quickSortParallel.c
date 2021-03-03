#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct arrays {
    int p0_start;
    int p1_start;
    int p2_start;
    int p3_start;
    int part_size;
    int overall_size;
};

int step = 1;

void printArray(int array[],int size);
struct arrays divideArray(int* array, int size);
int partition (int left, int right, int array[]);
void quicksort (int left, int right, int array[]);
void swap(int* a, int* b);
void sortArray(int* array, int start, int end);


int main(int argc, const char* argv[]) {

    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);
    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n"); 
   
    struct arrays divided_array = divideArray(array,size);
    // printf("Sorted array: ");
    // printArray(array,size);
    printf("first part of the array:");
    printArray(array,8);
    printf("\n");
    quicksort(divided_array.p0_start,divided_array.part_size,array);
    printf("first part of the array sorted:");
    printArray(array,8);
    printf("\n");
    printf("whole array:");
    printArray(array,size);
    printf("\n");
    printf("\n");



    return 0;

}

void sortArray(int* array, int start, int end, int proces_id){
    quicksort(start,end,array);

    if((proces_id == 0) && (step == 1))
    {
        
    }
}

void quicksort (int left, int right, int array[]){


    if (left < right)
    {
        int p = partition(left, right, array);
        quicksort(left, p -1, array);
        quicksort(p + 1, right, array);
    }
}

int partition (int left, int right, int array[]) {
    int pivot = array[right];
    int i = left - 1;

    for (int j = left; j <= right -1; j++)
    {
        if(array[j] < pivot)
        {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1],&array[right]);
    return (i + 1);
}

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

struct arrays divideArray(int* array, int size){
    struct arrays result;
    result.overall_size = size;
    result.part_size =  size / 4;
    result.p0_start = 0;
    result.p1_start = result.part_size;
    result.p2_start = result.part_size*2;
    result.p3_start = result.part_size*3;
    return result;
}



void printArray(int array[],int size){
    for(int i = 0; i < size; i++)
    {
        printf("%d",array[i]);
        printf(" ");
    }
}
