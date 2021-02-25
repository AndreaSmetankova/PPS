#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int partition (int left, int right, int array[]);
void quicksort (int left, int right, int array[]);
void swap(int* a, int* b);
void printArray(int array[],int size);


int main(int argc, const char* argv[]) {

    int array[] = {11,22,3,4,66,77,865,987, 43, 1, -3};
    int size = sizeof(array) / sizeof(array[0]);
    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n"); 
    quicksort(0,size -1, array);
    printf("Sorted array: ");
    printArray(array,size);
    printf("\n");



    return 0;

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

void printArray(int array[],int size){
    for(int i = 0; i < size; i++)
    {
        printf("%d",array[i]);
        printf(" ");
    }
}
