#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXSIZE 1000

struct arrays {
    int p0_array[MAXSIZE];
    int p0_size;
    int p1_array[MAXSIZE];
    int p1_size;
    int p2_array[MAXSIZE];
    int p2_size;
    int p3_array[MAXSIZE];
    int p3_size;
};

struct median_division {
    int lower_part[MAXSIZE];
    int lower_size;
    int higher_part[MAXSIZE];
    int higher_size;
};

struct arguments{
    int* array;
    int size;;
    int proces_id;
};
struct changable_parts{
    struct median_division p0;
    struct median_division p1;
    struct median_division p2;
    struct median_division p3;
};
void printArray(int array[], int size);
int partition (int left, int right, int array[]);
void quicksort (int left, int right, int array[]);
void swap(int* a, int* b);
struct arrays divideArray(int* array, int size);
int copyArray(int* source_array, int* destination_array, int size, int start);
struct median_division medianDivision (int* array, int size, int median);
void hyperQuickSort(void* arguments);
void switchParts (int proces_id);
void mergeArrays();
void printParts();



int step = 1;
int median1,median2;
struct changable_parts parts;
pthread_mutex_t lock;
pthread_cond_t cond;
unsigned int ready = 0;
unsigned int ready2 = 0;
struct arguments arg0,arg1,arg2,arg3;
unsigned int done;

int main(int argc, const char* argv[]) {
    
    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);
    struct arrays proces_arrays = divideArray(array, size);
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    
    arg0.array = proces_arrays.p0_array;
    arg0.size = proces_arrays.p0_size;
    arg0.proces_id = 0;

    arg1.array = proces_arrays.p1_array;
    arg1.size = proces_arrays.p1_size;
    arg1.proces_id = 1;

    arg2.array = proces_arrays.p2_array;
    arg2.size = proces_arrays.p2_size;
    arg2.proces_id = 2;

    arg3.array = proces_arrays.p3_array;
    arg3.size = proces_arrays.p3_size;
    arg3.proces_id = 3;

    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n");

    for(int i = 0; i < 2; i++)
    {
        done = 0;
        pthread_cond_init(&cond, NULL);
        pthread_create(&thread_id0,NULL,(void*)hyperQuickSort, (void*)&arg0);
        sleep(1);
        pthread_create(&thread_id2,NULL,(void*)hyperQuickSort, (void*)&arg2);
        sleep(1);
        pthread_create(&thread_id1,NULL,(void*)hyperQuickSort, (void*)&arg1);
        sleep(1);
        pthread_create(&thread_id3,NULL,(void*)hyperQuickSort, (void*)&arg3);
        

        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        printf("After threads %i\n", i + 1); 

        step++;
        
    }
    
    printf("array p0: ");
    printArray(arg0.array, arg0.size);
    printf("\n"); 
    
    printf("array p1: ");
    printArray(arg1.array, arg1.size);
    printf("\n"); 

    printf("array p2: ");
    printArray(arg2.array, arg3.size);
    printf("\n"); 
    
    printf("array p3: ");
    printArray(arg3.array, arg3.size);
    printf("\n"); 

    int final_size = arg0.size + arg1.size + arg2.size + arg3.size;
    int result[final_size];
    int i = 0;
    int j = 0;

    while( i < arg0.size)
    {
        result[i] = arg0.array[i];
        i++;
    }
    while (j < arg1.size)
    {
        result[i] = arg1.array[j];
        i++;
        j++;
    }
    j = 0;
    while (j < arg2.size)
    {
        result[i] = arg2.array[j];
        i++;
        j++;
    }
    j = 0;
    while (j < arg3.size)
    {
        result[i] = arg3.array[j];
        i++;
        j++;
    }

    printf("final: ");
    printArray(result,final_size);
    printf("\n");
    

    return 0;
}


void hyperQuickSort(void* arguments){
    struct arguments *args = (struct arguments *) arguments;
    quicksort(0, args->size - 1, args->array);
    printf("proces_id = %i\n", args->proces_id);
    if (step == 1)
    {
        if(args->proces_id == 0)
        {
            pthread_mutex_lock(&lock);
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            printf("ready\n");
            pthread_cond_broadcast(&cond);
            done++;
            pthread_mutex_unlock(&lock);

        } else
        {
            
           switch (args->proces_id)
           {
           case 1:
               pthread_mutex_lock(&lock);
               while(ready != 1)
               {
                   printf("waiting 1 ...\n");
                   pthread_cond_wait(&cond, &lock);
               }
               printf("done 1\n");
               parts.p1 = medianDivision (args->array, args->size, median1);
               done++;
               pthread_mutex_unlock(&lock);
               break;
           case 2:
               pthread_mutex_lock(&lock);
               while(ready != 1)
               {
                   printf("waiting 2...\n");
                   pthread_cond_wait(&cond, &lock);
               }
               printf("done 2\n");
               parts.p2 = medianDivision (args->array, args->size, median1);
               switchParts(args->proces_id);
               done++;
               pthread_mutex_unlock(&lock);
               break;
           case 3:
               pthread_mutex_lock(&lock);
               while(ready != 1)
               {
                   printf("waiting 3...\n");
                   pthread_cond_wait(&cond, &lock);
               }
               printf("done 3\n");
               parts.p3 = medianDivision (args->array, args->size, median1);
               switchParts(args->proces_id);
               done++;
               pthread_mutex_unlock(&lock);
               break;
           
           default:
               break;
           }
           
            
        }
        

    }else if(step == 2)
    {
        
        switch (args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 2;
            printf("ready\n");
            pthread_cond_broadcast(&cond);
            done++;
            pthread_mutex_unlock(&lock);
            break;
        case 1:
            pthread_mutex_lock(&lock);
               while(ready != 2)
               {
                   printf("waiting 1 ...\n");
                   pthread_cond_wait(&cond, &lock);
               }
               printf("done 1\n");
               parts.p1 = medianDivision (args->array, args->size, median1);
               switchParts(args->proces_id);
               done++;
               pthread_mutex_unlock(&lock);
               break;
        case 2:
            pthread_mutex_lock(&lock);
            median2 =args->array[(args->size - 1)/2];
            parts.p2 = medianDivision (args->array, args->size, median2);
            ready2 = 3;
            printf("ready\n");
            pthread_cond_broadcast(&cond);
            done++;
            pthread_mutex_unlock(&lock);
            break;
        case 3:
            pthread_mutex_lock(&lock);
               while(ready2 != 3)
               {
                   printf("waiting 3 ...\n");
                   pthread_cond_wait(&cond, &lock);
               }
               printf("done 1\n");
               parts.p3 = medianDivision (args->array, args->size, median2);
               switchParts(args->proces_id);
               done++;
               pthread_mutex_unlock(&lock);
               break;
        default:
            break;
        }
    }
    if(done == 4)
    {
        printf("done = %i\n", done);
        mergeArrays();
    }
    
    
}

void mergeArrays(){
    int merged_size;
    int result[MAXSIZE];
    int i;
    printf("merging step = %i....\n", step);
    
        printf("\n");
        merged_size =  parts.p0.lower_size + parts.p0.higher_size;
        printf("merged size = %i\n", merged_size);
        i = 0;
        while( i < parts.p0.lower_size )
        {
            result[i] = parts.p0.lower_part[i];
            i++;
        }
        for(int j = 0; j <  parts.p0.higher_size; j++ )
        {
            result[i] = parts.p0.higher_part[j];
            i++;
        }
        
        copyArray(result,arg0.array,merged_size,0);
        arg0.size = merged_size;
        quicksort(0,arg0.size -1, arg0.array);

    
        printArray(result, merged_size);
        printf("\n");
        printArray(arg0.array, arg0.size);
        printf("\n\n");
        
        
    
        merged_size =  parts.p1.lower_size + parts.p1.higher_size;
        i = 0;
        for(i; i < parts.p1.lower_size ; i++)
        {
            result[i] = parts.p1.lower_part[i];
        }
        for(int j = 0; j <  parts.p1.higher_size; j++ )
        {
            result[i] = parts.p1.higher_part[j];
            i++;
        }
        
        printArray(result, merged_size);
        printf("\n");
        copyArray(result,arg1.array,merged_size,0);
        arg1.size = merged_size;
        quicksort(0,arg1.size -1, arg1.array);
        
        
   
        merged_size =  parts.p2.lower_size + parts.p2.higher_size;
        i = 0;
        for(i; i < parts.p2.lower_size ; i++)
        {
            result[i] = parts.p2.lower_part[i];
        }
        for(int j = 0; j <  parts.p2.higher_size; j++ )
        {
            result[i] = parts.p2.higher_part[j];
            i++;
        }
        
        copyArray(result,arg2.array,merged_size,0);
        printf("\n");
        arg2.size = merged_size;
        quicksort(0,arg2.size -1, arg2.array);
      
        
    
        merged_size =  parts.p3.lower_size + parts.p3.higher_size;
        i = 0;
        for(i; i < parts.p3.lower_size ; i++)
        {
            result[i] = parts.p3.lower_part[i];
        }
        for(int j = 0; j <  parts.p3.higher_size; j++ )
        {
            result[i] = parts.p3.higher_part[j];
            i++;
        }
       
        printArray(result, merged_size);
        printf("\n");
        copyArray(result,arg3.array,merged_size,0);
        arg3.size = merged_size;
        quicksort(0,arg3.size -1, arg3.array);
    
   
    
    printf("\n+++++++++++++++++++++++++++++++++++++++\n");
    printArray(arg0.array, arg0.size);
    printf("\n");
    printArray(arg1.array, arg1.size);
    printf("\n");
    printArray(arg2.array, arg2.size);
    printf("\n");
    printArray(arg3.array, arg3.size);
    printf("\n+++++++++++++++++++++++++++++++++++++++++++\n");
}

void printParts(){
     printf("array lower p0: ");
        printArray(parts.p0.lower_part, parts.p0.lower_size);
        printf("\n"); 

        printf("array higher p0: ");
        printArray(parts.p0.higher_part, parts.p0.higher_size);
        printf("\n"); 

        printf("array lower p1: ");
        printArray(parts.p1.lower_part, parts.p1.lower_size);
        printf("\n"); 

        printf("array higher p1: ");
        printArray(parts.p1.higher_part, parts.p1.higher_size);
        printf("\n");

        printf("array lower p2: ");
        printArray(parts.p2.lower_part, parts.p2.lower_size);
        printf("\n"); 

        printf("array higher p2: ");
        printArray(parts.p2.higher_part, parts.p2.higher_size);
        printf("\n");

        printf("array lower p3: ");
        printArray(parts.p3.lower_part, parts.p3.lower_size);
        printf("\n"); 

        printf("array higher p3: ");
        printArray(parts.p3.higher_part, parts.p3.higher_size);
        printf("\n");
}

void switchParts (int proces_id){
   if(step == 1)
   {
       if(proces_id == 2)
       {
            printf("switching parts...\n");
            int tmp [MAXSIZE];
            int tmp_size =  copyArray(parts.p0.higher_part, tmp, parts.p0.higher_size, 0);
            printf("tmp array: ");
            printArray(tmp, tmp_size);
            printf("\n");
            parts.p0.higher_size = copyArray(parts.p2.lower_part, parts.p0.higher_part, parts.p2.lower_size, 0);
            printf("p0 higher array: ");
            printArray(parts.p0.higher_part, parts.p0.higher_size);
            printf("\n");
            parts.p2.lower_size = copyArray(tmp, parts.p2.lower_part, tmp_size, 0);
       } else if (proces_id == 3)
       {
           printf("switching parts...\n");
            int tmp [MAXSIZE];
            int tmp_size =  copyArray(parts.p1.higher_part, tmp, parts.p1.higher_size, 0);
            printf("tmp array: ");
            printArray(tmp, tmp_size);
            printf("\n");
            parts.p1.higher_size = copyArray(parts.p3.lower_part, parts.p1.higher_part, parts.p3.lower_size, 0);
            printf("p0 higher array: ");
            printArray(parts.p1.higher_part, parts.p1.higher_size);
            printf("\n");
            parts.p3.lower_size = copyArray(tmp, parts.p3.lower_part, tmp_size, 0);
       }
       

   }else
   {
        if(proces_id == 1)
       {
            printf("switching parts...\n");
            int tmp [MAXSIZE];
            int tmp_size =  copyArray(parts.p0.higher_part, tmp, parts.p0.higher_size, 0);
            printf("tmp array: ");
            printArray(tmp, tmp_size);
            printf("\n");
            parts.p0.higher_size = copyArray(parts.p1.lower_part, parts.p0.higher_part, parts.p1.lower_size, 0);
            printf("p0 higher array: ");
            printArray(parts.p0.higher_part, parts.p0.higher_size);
            printf("\n");
            parts.p1.lower_size = copyArray(tmp, parts.p1.lower_part, tmp_size, 0);
       } else if (proces_id == 3)
       {
           printf("switching parts...\n");
            int tmp [MAXSIZE];
            int tmp_size =  copyArray(parts.p2.higher_part, tmp, parts.p2.higher_size, 0);
            printf("tmp array: ");
            printArray(tmp, tmp_size);
            printf("\n");
            parts.p2.higher_size = copyArray(parts.p3.lower_part, parts.p2.higher_part, parts.p3.lower_size, 0);
            printf("p2 higher array: ");
            printArray(parts.p2.higher_part, parts.p2.higher_size);
            printf("\n");
            parts.p3.lower_size = copyArray(tmp, parts.p3.lower_part, tmp_size, 0);
       }
   } 
}

struct median_division medianDivision (int* array, int size, int median){
    struct median_division result;
    int higher_index = 0;

    if(array[0] > median)
    {
        result.higher_size = size;
        copyArray(array, result.higher_part, size, 0);
        copyArray(array, result.lower_part, -1, 0);
        result.lower_size = 0;
        return result;

    }

    for(int i = 0; i < size; i++)
    {
        if (array[i] > median)
        {
            higher_index = i;
            break;
        }
    }
    // printf("higher index %i\n", higher_index);
    result.lower_size = copyArray(array, result.lower_part, higher_index, 0);
    result.higher_size = copyArray(array, result.higher_part,size, higher_index);
    return result;
}

struct arrays divideArray(int* array, int size){
    struct arrays result;
    int part_size = size /4;
    result.p0_size = copyArray(array, result.p0_array, part_size, 0);
    result.p1_size =copyArray(array, result.p1_array, part_size * 2, part_size);
    result.p2_size =copyArray(array, result.p2_array, part_size * 3, part_size * 2);
    result.p3_size =copyArray(array, result.p3_array, size , part_size * 3);
    
    return result;

}

int copyArray(int* source_array, int* destination_array, int size, int start){
    int j = 0;
    for(int i = start; i < size; i++)
    {
        destination_array[j] = source_array[i];
        j++;
    }
    
    return j ;
}

void printArray(int array[], int size){
    for(int i = 0; i < size; i++)
    {
        printf("%d",array[i]);
        printf(" ");
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
