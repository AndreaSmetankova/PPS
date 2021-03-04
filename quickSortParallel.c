#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct arrays {
    int p0_start;
    int p1_start;
    int p2_start;
    int p3_start;
    int part_size;
    int overall_size;
};

struct arguments{
    int* array;
    int start;
    int end;
    int proces_id;
};

int step = 1;
pthread_mutex_t mutex;
void printArray(int array[],int size);
struct arrays divideArray(int* array, int size);
int partition (int left, int right, int array[]);
void quicksort (int left, int right, int array[]);
void swap(int* a, int* b);
void sortArray(void* arguments);


int main(int argc, const char* argv[]) {

    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    struct arrays divided_array = divideArray(array,size);
    struct arguments arg0,arg1,arg2,arg3;
    

    arg0.array = array;
    arg0.start = divided_array.p0_start;
    arg0.end = divided_array.p0_start + divided_array.part_size;
    arg0.proces_id = 0;

    arg1.array = array;
    arg1.start = divided_array.p1_start;
    arg1.end = divided_array.p1_start + divided_array.part_size;
    arg1.proces_id = 1;

    arg2.array = array;
    arg2.start = divided_array.p2_start;
    arg2.end = divided_array.p2_start + divided_array.part_size;
    arg2.proces_id = 2;

    arg3.array = array;
    arg3.start = divided_array.p3_start;
    arg3.end = divided_array.p3_start + divided_array.part_size;
    arg3.proces_id = 3;


    for(int i = 0; i < 2; i++)
    {
        pthread_create(&thread_id0,NULL,(void*)sortArray, (void*)&arg0);
        pthread_create(&thread_id1,NULL,(void*)sortArray, (void*)&arg1);
        pthread_create(&thread_id2,NULL,(void*)sortArray, (void*)&arg2);
        pthread_create(&thread_id3,NULL,(void*)sortArray, (void*)&arg3);
        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        printf("After threads %i\n", i + 1);
        step++;
        
    }
    

    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n"); 
   
   
    // printf("Sorted array: ");
    // // printArray(array,size);
    // printf("first part of the array:");
    // printArray(array,8);
    // printf("\n");
    // quicksort(divided_array.p0_start,divided_array.part_size,array);
    // printf("first part of the array sorted:");
    // printArray(array,8);
    // printf("\n");


    printf("whole array:");
    printArray(array,size);
    printf("\n");
    printf("\n");



    return 0;

}

void sortArray(void * arguments){
    struct arguments *args = (struct arguments *) arguments;
    quicksort(args->start, args->end, args->array);
    
    if (step == 1)
    {
        if(args->proces_id == 0)
        {
           printf("1. proces 0 and expected step 1 acctual step %i \n", step);  
        } else
        {
            printf("2. proces %i and expected step 1 acctual step %i\n",args->proces_id, step);
        }
    }else if(step == 2)
    {
        if(args->proces_id == 0 || args->proces_id == 2)
        {
            printf("3. proces %i and expected step 2 acctual step %i\n", args->proces_id, step);
        }else
        {
            printf("4. proces %i and expected step 2 acctual step %i\n", args->proces_id, step);
        }
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
