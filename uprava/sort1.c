#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// #include <unistd.h>
#include <time.h> 

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
struct can_switch {
    int p0;
    int p1;
    int p2;
    int p3; 
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
void switchPartsParallel(struct arguments* args);
void merge(int* final_array, int size);
void printParts();

pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_cond_t cond;
pthread_cond_t cond2;
int step = 1;
int median1,median2;
unsigned int ready = 0;
unsigned int ready2 = 0;
struct changable_parts parts;
struct arguments arg;
int* p_tmp0;
int* p_tmp1;
int* p_tmp2;
int* p_tmp3;
struct can_switch can_switch;
int* final_array;

int main(int argc, const char* argv[]) {
    //TODO: citanie pola s file
    int array[] = {75,91,15,64,21,8,88,54,50,12,47,72,65,54,66,22,83,66,67,0,70,98,99,82,20,40,89,47,19,61,86,85};
    int size = sizeof(array) / sizeof(array[0]);

    // printf("-------------------------------------QUICKSORT-------------------------\n");
    // printf("Unsorted array: ");
    // printArray(array,size);
    // printf("\n");

    // clock_t t; 
    // t = clock();
    // quicksort(0, size - 1, array);
    // t = clock() - t; 
    // double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
  
    // printf("quicksort() took %f seconds to execute \n", time_taken);


    // printf("Sorted array: ");
    // printArray(array,size);
    // printf("\n");
    // printf("-------------------------------------QUICKSORT-------------------------\n");

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

void parallelQuickSort(int* array, int size){
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond2, NULL);
    
    arg = divideArray(array, size);
    final_array = array;
    can_switch.p0 = 0;
    can_switch.p1 = 0;
    can_switch.p2 = 0;
    can_switch.p3 = 0;

     for(int i = 0; i < 2; i++)
    {
        // printf("start\n");
        printf("array before ");
        printArray(array,size);
        printf("\n");
        
        pthread_create(&thread_id0,NULL,(void*)hyperQuickSort, (void*)&arg.p0);
        pthread_create(&thread_id1,NULL,(void*)hyperQuickSort, (void*)&arg.p1);
        pthread_create(&thread_id2,NULL,(void*)hyperQuickSort, (void*)&arg.p2);
        pthread_create(&thread_id3,NULL,(void*)hyperQuickSort, (void*)&arg.p3);
        
        // printf("\nbefore thread joins\n");

        printf("%i\n", pthread_join(thread_id0,NULL));
        printf("%i\n", pthread_join(thread_id1,NULL));
        printf("%i\n", pthread_join(thread_id2,NULL));
        printf("%i\n", pthread_join(thread_id3,NULL));

        // printf("\nafter thread joins\n");
        
        // printf("\nafter threads\n");
        // switchParts(&arg, array, size);
        // printf("\n-------------step = %i-----------------------\n", step);
        // printParts();
        // printf("\n------------------------------------\n");
        printf("\n---------------------------------------\n");
        merge(array,size);
        printf("\n----------- ----------------------------\n");


        printf("array after ");
        printArray(array,size);
        printf("\n");
        step++;
        
    }

    free( p_tmp0);
    free( p_tmp1);
    free( p_tmp2);
    free( p_tmp3);

}

void hyperQuickSort(void* arguments){
    struct proces* args = (struct proces *) arguments;
    quicksort(0, args->size - 1, args->array);
    // printf("proces = %i\n", args->proces_id);
    
    if(step == 1)
    {
        switch(args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            pthread_cond_broadcast(&cond);
            can_switch.p0 = 1;
            if(can_switch.p0 == 1 && can_switch.p2 == 1)
            {
                // printf("STEP = 1 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 1\n", args->proces_id);
            }
            
            pthread_mutex_unlock(&lock);
            
            break;
        case 1:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            can_switch.p1 = 1;
            if(can_switch.p1 == 1 && can_switch.p3 == 1)
            {
                // printf("STEP = 1 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 1\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p2 = medianDivision (args->array, args->size, median1);
            can_switch.p2 = 1;
            if(can_switch.p0 == 1 && can_switch.p2 == 1)
            {
                // printf("STEP = 1 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 1\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 3:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p3 = medianDivision (args->array, args->size, median1);
            can_switch.p3 = 1;
            if(can_switch.p1 == 1 && can_switch.p3 == 1)
            {
                // printf("STEP = 1 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 1\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock);
            
            break;
       
        default:
            break;

        }

        
    } else
    {
        switch(args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            can_switch.p0 = 2;
            pthread_cond_broadcast(&cond);
            if(can_switch.p0 == 2 && can_switch.p1 == 2)
            {
                // printf("STEP = 2 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 2\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock);
            
            break;
            
        case 1:
            pthread_mutex_lock(&lock);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            can_switch.p1 = 2;
            if(can_switch.p0 == 2 && can_switch.p1 == 2)
            {
                // printf("STEP = 2 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 2\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock2);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            median2 =args->array[(args->size - 1)/2];
            // printf("median = %i", median2);
            ready2 = 1;
            parts.p2 = medianDivision (args->array, args->size, median2);
            can_switch.p2 = 2;
            pthread_cond_broadcast(&cond2);
            if(can_switch.p2 == 2 && can_switch.p3 == 2)
            {
                // printf("STEP = 2 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 2\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock2);
            break;
        case 3:
            pthread_mutex_lock(&lock2);
            printf("after quicksort ");
            printArray(args->array, args->size);
            printf("\n");
            while (ready2 != 1)
            {
               pthread_cond_wait(&cond2, &lock2);
            }
            parts.p3 = medianDivision (args->array, args->size, median2);
            can_switch.p3 = 2;
            if(can_switch.p2 == 2 && can_switch.p3 == 2)
            {
                // printf("STEP = 2 p0 switch = %i p1 switch = %i p2 switch = %i p3 switch = %i\n", can_switch.p0, can_switch.p1, can_switch.p2, can_switch.p3);
                switchPartsParallel(&arg);
                // printf("switching done for proces = %i in step 2\n", args->proces_id);
            }
            pthread_mutex_unlock(&lock2);
            break;
        default:
            break;

        }
    }
    
    // printf("done hzperquicksort proces %i\n", args->proces_id);
    pthread_exit(0);
    
}

void switchPartsParallel(struct arguments* args){

    printf("\n-------------switching step = %i-----------------------\n", step);
    // printParts();
    // printf("\n------------------------------------\n");
    if(step == 1)
    {
        if(can_switch.p0 == 1 && can_switch.p2 == 1)
        {
            // printf("switching 0 a 2\n");
            //new p0 array
            args->p0.size = parts.p0.lower_size + parts.p2.lower_size;
            // int tmp0[args->p0.size];
            p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            int i = 0;
            while (  i < parts.p0.lower_size)
            {
                p_tmp0[i] = parts.p0.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p2.lower_size; j++)
            {
                p_tmp0[i] = parts.p2.lower_part[j];
                i++;
            }
            // printf("step 1 -------- p0 ");
            // printArray(p_tmp0, args->p0.size);
            // printf("\n");
            // args->p0.array = &final_array[0];

            //new p2 array
            // args->p2.array = &final_array[args->p1.size + args->p0.size];
            args->p2.size = parts.p2.higher_size + parts.p0.higher_size;
            // int tmp2[args->p2.size];
            p_tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            for ( i = 0; i < parts.p2.higher_size ; i++)
            {
                p_tmp2[i] = parts.p2.higher_part[i];
            }
            for (int j = 0; j < parts.p0.higher_size; j++)
            {
                p_tmp2[i] = parts.p0.higher_part[j];
                i++;
            }
            // printf("step 1 -------- p2 ");
            // printArray(p_tmp2, args->p2.size);
            // printf("\n"); 
        }

        if(can_switch.p1 == 1 && can_switch.p3 == 1)
        {
            // printf("switching 1 a 3\n");
           //new p1 array
            // args->p1.array = &final_array[args->p0.size];
            args->p1.size = parts.p1.lower_size + parts.p3.lower_size;
            // int tmp1[args->p1.size];
            int i;
            p_tmp1 = (int*)calloc(args->p1.size, sizeof(int));
            for ( i = 0; i < parts.p1.lower_size ; i++)
            {
                p_tmp1[i] = parts.p1.lower_part[i];
            }
            for (int j = 0; j < parts.p3.lower_size; j++)
            {
                p_tmp1[i] = parts.p3.lower_part[j];
                i++;
            } 
            // printf("step 1 -------- p1 ");
            // printArray(p_tmp1, args->p1.size);
            // printf("\n");

            //new p3 array
            // args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
            args->p3.size = parts.p3.higher_size + parts.p1.higher_size;
            // int tmp3[args->p3.size];
            p_tmp3 = (int*)calloc(args->p3.size, sizeof(int));
            for ( i = 0; i < parts.p3.higher_size ; i++)
            {
                p_tmp3[i] = parts.p3.higher_part[i];
            }

            for (int j = 0; j < parts.p1.higher_size; j++)
            {
                p_tmp3[i] = parts.p1.higher_part[j];
                i++;
            }
            // printf("step 1 -------- p3 ");
            // printArray(p_tmp3, args->p3.size);
            // printf("\n");
        }

        args->p0.array = &final_array[0];
        args->p2.array = &final_array[args->p1.size + args->p0.size];
        args->p1.array = &final_array[args->p0.size];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];

    } else
    {
        if(can_switch.p0 == 2 && can_switch.p1 == 2)
        {
            // printf("switching 0 a 1\n");
            //new p0 array
            args->p0.size = parts.p0.lower_size + parts.p1.lower_size;
            // int tmp0[args->p0.size];
            // p_tmp0 = (int*)realloc(p_tmp0,args->p0.size);
            p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            int i = 0;
            while ( i < parts.p0.lower_size)
            {
                p_tmp0[i] = parts.p0.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p1.lower_size; j++)
            {
                p_tmp0[i] = parts.p1.lower_part[j];
                i++;
            }
            // printf("quicksorting 0 \n");
            quicksort(0,args->p0.size - 1, p_tmp0);
            // printf("step 2 -------- p0 ");
            // printArray(p_tmp0, args->p0.size);
            // printf("\n");
            // args->p0.array = &final_array[0];
            // printf("=====================switching first end===============\n");
            
            //new p1 array
            // printf("switching 0 a 1\n");
            // args->p1.array = &final_array[args->p0.size];
            args->p1.size = parts.p1.higher_size + parts.p0.higher_size;
            // int tmp1[args->p1.size];
            // p_tmp1 = (int*)realloc(p_tmp1,args->p1.size);
            p_tmp1 = (int*)calloc(args->p1.size, sizeof(int));
            for ( i = 0; i < parts.p1.higher_size ; i++)
            {
                p_tmp1[i] = parts.p1.higher_part[i];
            }
            for (int j = 0; j < parts.p0.higher_size; j++)
            {
                p_tmp1[i] = parts.p0.higher_part[j];
                i++;
            }
            // printf("quicksorting 1 \n");
            quicksort(0,args->p1.size - 1, p_tmp1);
            // printf("step 2 -------- p1 ");
            // printArray(p_tmp1, args->p1.size);
            // printf("\n");
            // printf("=====================switching first end===============\n");
        }

        if(can_switch.p2 == 2 && can_switch.p3 == 2)
        {
            // printf("switching 2 a 3\n");
            //new p2 array
            // args->p2.array = &final_array[args->p1.size + args->p0.size];
            args->p2.size = parts.p2.lower_size + parts.p3.lower_size;
            // int tmp2[args->p2.size];
            int i;
            // p_tmp2 = (int*)realloc(p_tmp2,args->p2.size);
            p_tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            for ( i = 0; i < parts.p2.lower_size ; i++)
            {
                p_tmp2[i] = parts.p2.lower_part[i];
            }
            for (int j = 0; j < parts.p3.lower_size; j++)
            {
                p_tmp2[i] = parts.p3.lower_part[j];
                i++;
            } 
            // printf("quicksorting 2 \n");
            quicksort(0,args->p2.size - 1, p_tmp2);  
            // printf("step 2 -------- p2 ");
            // printArray(p_tmp2, args->p2.size);
            // printf("\n");

            //new p3 array;
            // printf("2x switching 2 a 3\n");
            // args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
            args->p3.size = parts.p3.higher_size + parts.p2.higher_size;
            // int tmp3[args->p3.size];
            p_tmp3 = (int*)calloc(args->p3.size, sizeof(int));
            for ( i = 0; i < parts.p3.higher_size ; i++)
            {
                p_tmp3[i] = parts.p3.higher_part[i];
            }

            for (int j = 0; j < parts.p2.higher_size; j++)
            {
                p_tmp3[i] = parts.p2.higher_part[j];
                i++;
            }
            // printf("quicksorting 3 \n");
            quicksort(0,args->p3.size - 1, p_tmp3); 
            // printf("step 2 -------- p3 ");
            // printArray(p_tmp3, args->p3.size);
            // printf("\n"); 
        }
        printf("p0 size = %i p1 size = %i p2 size = %i p3 size = %i \n", args->p0.size, args->p1.size, args->p2.size, args->p3.size);
        args->p0.array = &final_array[0];
        args->p1.array = &final_array[args->p0.size +1];
        args->p2.array = &final_array[args->p1.size + args->p0.size +1];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size +1];
    }

    // printf("\n***********end************\n");
}

void merge(int* final_array, int size)
{
    // printf("merging....\n\n");
    // printf("p0 = ");
    // printArray(p_tmp0, arg.p0.size);
    // printf("\n");
    // printf("p1 = ");
    // printArray(p_tmp1, arg.p1.size);
    // printf("\n");
    // printf("p2 = ");
    // printArray(p_tmp2, arg.p2.size);
    // printf("\n");
    // printf("p3 = ");
    // printArray(p_tmp3, arg.p3.size);
    // printf("\n");

    int start = 0;
    while( start < arg.p0.size)
    {
        final_array[start] = p_tmp0[start];
        start++;
    }

    for(int i = 0; i < arg.p1.size; i++)
    {
        final_array[start] = p_tmp1[i];
        start++;
    }
    for(int i = 0; i < arg.p2.size; i++)
    {
        final_array[start] = p_tmp2[i];
        start++;
    }
    for(int i = 0; i < arg.p3.size; i++)
    {
        final_array[start] = p_tmp3[i];
        start++;
    }
    printf("merging to final array\n");
    printArray(final_array,size);
    printf("\n");
}

/*
void switchParts(struct arguments* args, int * array, int size){
//TODO: sparvit tak aby sa to vzkonavalo paralelne
    if(step == 1)
    {   
        //new p0 array
        args->p0.size = parts.p0.lower_size + parts.p2.lower_size;
        int tmp0[args->p0.size];
        int i = 0;
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

        //new p1 array
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

        //new p2 array
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
 
        //new p3 array
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

        //funkcia
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
       
    } else 
    {
        //new p0 array
        args->p0.size = parts.p0.lower_size + parts.p1.lower_size;
        int tmp0[args->p0.size];
        int i = 0;
        for ( i; i < parts.p0.lower_size; i++)
        {
            tmp0[i] = parts.p0.lower_part[i];
        }
        for (int j = 0; j < parts.p1.lower_size; j++)
        {
            tmp0[i] = parts.p1.lower_part[j];
            i++;
        }
        quicksort(0,args->p0.size - 1, tmp0);
        args->p0.array = &array[0];
        
        //new p1 array
        args->p1.array = &array[args->p0.size];
        args->p1.size = parts.p1.higher_size + parts.p0.higher_size;
        int tmp1[args->p1.size];
        for ( i = 0; i < parts.p1.higher_size ; i++)
        {
            tmp1[i] = parts.p1.higher_part[i];
        }
        for (int j = 0; j < parts.p0.higher_size; j++)
        {
            tmp1[i] = parts.p0.higher_part[j];
            i++;
        }
        quicksort(0,args->p1.size - 1, tmp1); 

        //new p2 array
        args->p2.array = &array[args->p1.size + args->p0.size];
        args->p2.size = parts.p2.lower_size + parts.p3.lower_size;
        int tmp2[args->p2.size];
        for ( i = 0; i < parts.p2.lower_size ; i++)
        {
            tmp2[i] = parts.p2.lower_part[i];
        }
        for (int j = 0; j < parts.p3.lower_size; j++)
        {
            tmp2[i] = parts.p3.lower_part[j];
            i++;
        } 
        quicksort(0,args->p2.size - 1, tmp2);  

        //new p3 array;
        args->p3.array = &array[args->p2.size + args->p1.size + args->p0.size];
        args->p3.size = parts.p3.higher_size + parts.p2.higher_size;
        int tmp3[args->p3.size];
        for ( i = 0; i < parts.p3.higher_size ; i++)
        {
            tmp3[i] = parts.p3.higher_part[i];
        }

        for (int j = 0; j < parts.p2.higher_size; j++)
        {
            tmp3[i] = parts.p2.higher_part[j];
            i++;
        }
        quicksort(0,args->p3.size - 1, tmp3);
        

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
       
    
      
    }
}
*/
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
    
    // printf("array = ");
    // printArray(array, size);
    // printf("\n");

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