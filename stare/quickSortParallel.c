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

struct median_division{
    int lower_start;
    int lower_end;
    int higher_start;
    int higher_end;
};

int step = 1;
pthread_mutex_t mutex;
int median1,median2;


void printArray(int array[], int start, int size);
struct arrays divideArray(int* array, int size);
int partition (int left, int right, int array[]);
void quicksort (int left, int right, int array[]);
void swap(int* a, int* b);
void sortArray(void* arguments);
struct median_division medianDivision (int* array, int start, int end, int median);


int main(int argc, const char* argv[]) {

    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    struct arrays divided_array = divideArray(array,size);
    struct arguments arg0,arg1,arg2,arg3;
    pthread_mutex_init(&mutex, NULL);

    arg0.array = array;
    arg0.start = divided_array.p0_start;
    arg0.end = divided_array.p0_start + divided_array.part_size ;
    arg0.proces_id = 0;

    arg1.array = array;
    arg1.start = divided_array.p1_start;
    arg1.end = divided_array.p1_start + divided_array.part_size;
    arg1.proces_id = 1;

    arg2.array = array;
    arg2.start = divided_array.p2_start;
    arg2.end = divided_array.p2_start + divided_array.part_size ;
    arg2.proces_id = 2;

    arg3.array = array;
    arg3.start = divided_array.p3_start;
    arg3.end = divided_array.p3_start + divided_array.part_size ;
    arg3.proces_id = 3;

    printf("Unsorted array: ");
    printArray(array,0,size);
    printf("\n");


    for(int i = 0; i < 2; i++)
    {
        pthread_create(&thread_id0,NULL,(void*)sortArray, (void*)&arg0);
        sleep(1);
        pthread_create(&thread_id2,NULL,(void*)sortArray, (void*)&arg2);
        sleep(1);
        pthread_create(&thread_id1,NULL,(void*)sortArray, (void*)&arg1);
        pthread_create(&thread_id3,NULL,(void*)sortArray, (void*)&arg3);
       
        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        printf("After threads %i\n", i + 1);
        step++;
        
    }
     
    return 0;

}
/*TODO: divide partial arrays acording to median and exchange between threads for both first and second step*/

void sortArray(void * arguments){
    struct arguments *args = (struct arguments *) arguments;
    quicksort(args->start, args->end, args->array);
    printf("proces_id = %i\n", args->proces_id);
    if (step == 1)
    {
        if(args->proces_id == 0)
        {
        //    pthread_mutex_lock(&mutex);
           printf("1. proces 0 and expected step 1 acctual step %i \n", step); 
           median1 = args->start + ((args->end - args->start)/2);
           printArray(args->array,args->start, args->end);
           printf("\n median = %i value = %i\n", median1, args->array[median1]);
        //    pthread_mutex_unlock(&mutex);
        } else
        {
            // pthread_mutex_lock(&mutex);
            
            struct median_division d_array = medianDivision (args->array, args->start, args->end, args->array[median1]);
            printf("divided array on median %i\n", median1);
            printf("lower: ");
            for (int i =d_array.lower_start; i < (d_array.lower_end +1); i++)
            {
                printf("%i ", args->array[i]);
            }
            printf("\n");
            printf("higher: ");
            for (int i =d_array.higher_start; i < (d_array.higher_end +1); i++)
            {
                printf("%i ", args->array[i]);
            }
            printf("\n");
            // printf("2. proces %i and expected step 1 acctual step %i\n",args->proces_id, step);
            // pthread_mutex_unlock(&mutex);
        }
    }else if(step == 2)
    {
        if(args->proces_id == 0 || args->proces_id == 2)
        {
            //not so good solution
            // pthread_mutex_lock(&mutex);
            if(args->proces_id == 0)
            {
                median1 = args->start + ((args->end - args->start)/2);
            }else{
                median2 = args->start + ((args->end - args->start)/2);
            }
            // printf("3. proces %i and expected step 2 acctual step %i\n", args->proces_id, step);
            // pthread_mutex_unlock(&mutex);
        }else
        {
            // pthread_mutex_lock(&mutex);
            // printf("4. proces %i and expected step 2 acctual step %i\n", args->proces_id, step);
            // pthread_mutex_unlock(&mutex);
        }
    }
}

struct median_division medianDivision (int* array, int start, int end, int median){
    struct median_division result;
    printf("median = %i\n",median);
    if(array[start] <= median)
    {
        result.lower_start = start;
    } else {
        result.lower_start = -1;
        result.lower_end = -1;
        result.higher_start = start;
        result.higher_end = end -1;

        return result;
    }

    for(int i = start; i < end; i++)
    {
        if (array[i] > median)
        {
            result.higher_start = i;
            result.higher_end = end -1;
            result.lower_end = i - 1;
            return result;
        }
    }
   
    return result;
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



void printArray(int array[], int start, int size){
    for(int i = start; i < size; i++)
    {
        printf("%d",array[i]);
        printf(" ");
    }
}
