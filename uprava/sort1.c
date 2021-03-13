#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXSIZE 1000


struct proces{
    int* array;
    int size;;
    int proces_id;
};

struct arguments {
    struct proces p0;
    struct proces p1;
    struct proces p2;
    struct proces p3;
    
};

struct median_division {
    int* lower_part;
    int lower_size;
    int* higher_part;
    int higher_size;
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

void parallelQuickSort(int* array, int size);
struct arguments divideArray(int* array, int size);
void hyperQuickSort(void* arguments);
struct median_division medianDivision (int* array, int size, int median);
void switchParts(struct arguments* args, int * arrray, int size);
int copyArray(int* source_array, int* destination_array, int size, int start);

pthread_mutex_t lock;
pthread_cond_t cond;
int step = 1;
unsigned int done;
int median1,median2;
unsigned int ready = 0;
unsigned int ready2 = 0;
struct changable_parts parts;
struct arguments arg;

int main(int argc, const char* argv[]) {
    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);

    printf("Unsorted array: ");
    printArray(array,size);
    printf("\n");

    parallelQuickSort(array,size);


    printf("Sorted array: ");
    printArray(array,size);
    printf("\n");

    return 0;
}

void parallelQuickSort(int* array, int size){
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    pthread_cond_init(&cond, NULL);
    
    arg = divideArray(array, size);

     for(int i = 0; i < 2; i++)
    {
        done = 0;
        
        pthread_create(&thread_id0,NULL,(void*)hyperQuickSort, (void*)&arg.p0);
        sleep(1);
        pthread_create(&thread_id1,NULL,(void*)hyperQuickSort, (void*)&arg.p1);
        pthread_create(&thread_id2,NULL,(void*)hyperQuickSort, (void*)&arg.p2);
        pthread_create(&thread_id3,NULL,(void*)hyperQuickSort, (void*)&arg.p3);
        
        

        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        printf("After threads %i\n", i + 1); 
        
        printf("------------------------\n");
        switchParts(&arg, array, size);
        printf("------------------------\n");
        
        printf("array p0: ");
        printArray(arg.p0.array, arg.p0.size);
        printf("\n"); 
        
        printf("array p1: ");
        printArray(arg.p1.array, arg.p1.size);
        printf("\n"); 

        printf("array p2: ");
        printArray(arg.p2.array, arg.p2.size);
        printf("\n"); 
        
        printf("array p3: ");
        printArray(arg.p3.array, arg.p3.size);
        printf("\n"); 
        printf("\n"); 
       

        step++;
        
    }

    

}

void hyperQuickSort(void* arguments){
    struct proces* args = (struct proces *) arguments;
    quicksort(0, args->size - 1, args->array);
    printf("proces %i\n",args->proces_id);
    if(step == 1)
    {
        switch(args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            // printf("ready\n");
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        case 1:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            
            // printf("p1 mdeian1 = %i\n", median1);
            parts.p1 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            
            // printf("p2 mdeian1 = %i\n", median1);
            parts.p2 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            break;
        case 3:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            
            // printf("p3 mdeian1 = %i\n", median1);
            parts.p3 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            
       
        default:
            break;

        }

    } else
    {
        switch(args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            median1 =args->array[(args->size - 1)/2];
            ready = 1;
            // printf("ready 2\n");
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
            
        case 1:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            
            // printf("p1 mdeian1 = %i\n", median1);
            pthread_mutex_unlock(&lock);
            break;
        case 2:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            median2 =args->array[(args->size - 1)/2];
            ready2 = 1;
            // printf("ready 2\n");
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        case 3:
            pthread_mutex_lock(&lock);
            // printArray(args->array, args->size);
            // printf("\n");
            while (ready2 != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            
            // printf("p3 mdeian1 = %i\n", median2);
            pthread_mutex_unlock(&lock);
            break;
        default:
            break;

        }
    }
    
    
}

void switchParts(struct arguments* args, int * array, int size){

    printf("\n++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("array p0: ");
    printArray(arg.p0.array, arg.p0.size);
    printf("\n"); 
        
    printf("array p1: ");
    printArray(arg.p1.array, arg.p1.size);
    printf("\n"); 

    printf("array p2: ");
    printArray(arg.p2.array, arg.p2.size);
    printf("\n"); 
        
    printf("array p3: ");
    printArray(arg.p3.array, arg.p3.size);
    printf("\n"); 
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("\n");

    if(step == 1)
    {   
        //new p0 array
        args->p0.size = parts.p0.lower_size + parts.p2.lower_size;
        int tmp0[args->p0.size];
        int i = 0;
        printf("p0 i = %i\n",i);
        for ( i; i < parts.p0.lower_size; i++)
        {
            tmp0[i] = parts.p0.lower_part[i];
        }
        for (int j = 0; j < parts.p2.lower_size; j++)
        {
            tmp0[i] = parts.p2.lower_part[j];
            i++;
        }
        args->p0.array = &array[0];
        printf("tmp = ");
        printArray(tmp0, args->p0.size);
        printf("\n");
        // quicksort(0,args->p0.size - 1, p0_tmp);
        // copyArray(p0_tmp, args->p0.array, args->p0.size, 0);
        

        // printf("\n");
        // printf("args.p0.size = %i args.p0.array = ", args->p0.size);
        // printArray(args->p0.array, args->p0.size);
        // printf("\n");

        //new p1 array
        printf("p1 i = %i\n", i);

        args->p1.array = &array[args->p0.size];
        args->p1.size = parts.p1.lower_size + parts.p3.lower_size;
        int tmp1[args->p1.size];
        for ( i = 0; i < parts.p1.lower_size ; i++)
        {
            tmp1[i] = parts.p1.lower_part[i];
        }
        for (int j = 0; j < parts.p3.lower_size; j++)
        {
            tmp1[i] = parts.p3.lower_part[j];
            i++;
        }
        printf("tmp = ");
        printArray(tmp1, args->p1.size);
        printf("\n");
        // quicksort(0, args->p1.size - 1, p1_tmp);
        // copyArray(p1_tmp, args->p1.array, args->p1.size, 0);
        

        // printf("args.p1.size = %i args.p1.array = ", args->p1.size);
        // printArray(args->p1.array, args->p1.size);
        // printf("\n");

        //new p2 array
        printf("p2 i = %i\n", i);
        args->p2.array = &array[args->p1.size + args->p0.size];
        args->p2.size = parts.p2.higher_size + parts.p0.higher_size;
        int tmp2[args->p2.size];
        for ( i = 0; i < parts.p2.higher_size ; i++)
        {
            tmp2[i] = parts.p2.higher_part[i];
        }
        for (int j = 0; j < parts.p0.higher_size; j++)
        {
            tmp2[i] = parts.p0.higher_part[j];
            i++;
        } 
        printf("tmp = ");
        printArray(tmp2, args->p2.size);
        printf("\n");
        // quicksort(0, args->p2.size - 1, p2_tmp);
        // copyArray(p2_tmp, args->p2.array, args->p2.size, 0);
        


        // printf("args.p2.size = %i args.p2.array = ", args->p2.size);
        // printArray(args->p2.array, args->p2.size);
        // printf("\n");

        //new p3 array
        printf("p3 i = %i\n", i);
        args->p3.array = &array[args->p2.size + args->p1.size + args->p0.size];
        args->p3.size = parts.p3.higher_size + parts.p1.higher_size;
        int tmp3[args->p3.size];
        for ( i = 0; i < parts.p3.higher_size ; i++)
        {
            tmp3[i] = parts.p3.higher_part[i];
        }

        for (int j = 0; j < parts.p1.higher_size; j++)
        {
            tmp3[i] = parts.p1.higher_part[j];
            i++;
        }
        printf("tmp = ");
        printArray(tmp3, args->p3.size);
        printf("\n");

        int start = 0;
        for(start; start < args->p0.size; start++)
        {
            array[start] = tmp0[start];
        }

        for(int i = 0; i < args->p1.size; i++)
        {
            array[start] = tmp1[i];
            start++;
        }
        for(int i = 0; i < args->p2.size; i++)
        {
            array[start] = tmp2[i];
            start++;
        }
        for(int i = 0; i < args->p3.size; i++)
        {
            array[start] = tmp3[i];
            start++;
        }
       
        // copyArray(tmp, array, size, 0);
        // printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
        // printArray(tmp, i);
        // printf("\n");
        // printArray(array,size);
        // printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

        // quicksort(0, args->p3.size - 1, p3_tmp); 
        // copyArray(p3_tmp, args->p3.array, args->p3.size, 0);
       
        
        // printf("args.p3.size = %i args.p3.array = ", args->p3.size);
        // printArray(args->p3.array, args->p3.size);
        // printf("\n");
        
           

       printf("##########################################################\n");
       printf("args.p0.size = %i args.p0.array = ", args->p0.size);
       printArray(args->p0.array, args->p0.size);
       printf("\n");
       printf("args.p1.size = %i args.p1.array = ", args->p1.size);
       printArray(args->p1.array, args->p1.size);
       printf("\n");
       printf("args.p2.size = %i args.p2.array = ", args->p2.size);
       printArray(args->p2.array, args->p2.size);
       printf("\n");
       printf("args.p3.size = %i args.p3.array = ", args->p3.size);
       printArray(args->p3.array, args->p3.size);
       printf("\n########################################################\n");
    } else 
    {
        
    }
}

struct median_division medianDivision (int* array, int size, int median){
    struct median_division result;
    int higher_index = 0;

    if(array[0] > median)
    {
        result.higher_size = size;
        result.higher_part = array;
        result.lower_part = NULL;
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
    result.higher_part = &array[higher_index];
    result.higher_size = size - higher_index;
    result.lower_part = &array[0];
    result.lower_size = higher_index;
    
    return result;
}

struct arguments divideArray(int* array, int size){
    struct arguments result;
    int part_size = size /4;
    //vyriesit problem pri poliach ktorych velkost nie je delitelna 4
    result.p0.array = &array[0];
    result.p0.size = part_size;
    result.p0.proces_id = 0;
    result.p1.array = &array[part_size];
    result.p1.size = part_size;
    result.p1.proces_id = 1;
    result.p2.array = &array[part_size * 2];
    result.p2.size = part_size;
    result.p2.proces_id = 2;
    result.p3.array = &array[part_size * 3];
    result.p3.size = size - 3 * part_size;
    result.p3.proces_id = 3;

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