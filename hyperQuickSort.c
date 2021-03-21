#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "quickSort.h"
#include "hyperQuickSort.h"

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
        pthread_create(&thread_id0,NULL,(void*)hyperQuickSort, (void*)&arg.p0);
        pthread_create(&thread_id1,NULL,(void*)hyperQuickSort, (void*)&arg.p1);
        pthread_create(&thread_id2,NULL,(void*)hyperQuickSort, (void*)&arg.p2);
        pthread_create(&thread_id3,NULL,(void*)hyperQuickSort, (void*)&arg.p3);

        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);


        merge(array,size);

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
    
    if(step == 1)
    {
        switch(args->proces_id)
        {
        case 0:
            pthread_mutex_lock(&lock);
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            pthread_cond_broadcast(&cond);
            can_switch.p0 = 1;
            if(can_switch.p0 == 1 && can_switch.p2 == 1)
            {
                switchPartsParallel(&arg);
            }
            
            pthread_mutex_unlock(&lock);
            
            break;
        case 1:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            can_switch.p1 = 1;
            if(can_switch.p1 == 1 && can_switch.p3 == 1)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p2 = medianDivision (args->array, args->size, median1);
            can_switch.p2 = 1;
            if(can_switch.p0 == 1 && can_switch.p2 == 1)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 3:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p3 = medianDivision (args->array, args->size, median1);
            can_switch.p3 = 1;
            if(can_switch.p1 == 1 && can_switch.p3 == 1)
            {
                switchPartsParallel(&arg);
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
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            can_switch.p0 = 2;
            pthread_cond_broadcast(&cond);
            if(can_switch.p0 == 2 && can_switch.p1 == 2)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock);
            
            break;
            
        case 1:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            can_switch.p1 = 2;
            if(can_switch.p0 == 2 && can_switch.p1 == 2)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock2);
            median2 =args->array[(args->size - 1)/2];
            ready2 = 1;
            parts.p2 = medianDivision (args->array, args->size, median2);
            can_switch.p2 = 2;
            pthread_cond_broadcast(&cond2);
            if(can_switch.p2 == 2 && can_switch.p3 == 2)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock2);
            break;
        case 3:
            pthread_mutex_lock(&lock2);
            while (ready2 != 1)
            {
               pthread_cond_wait(&cond2, &lock2);
            }
            parts.p3 = medianDivision (args->array, args->size, median2);
            can_switch.p3 = 2;
            if(can_switch.p2 == 2 && can_switch.p3 == 2)
            {
                switchPartsParallel(&arg);
            }
            pthread_mutex_unlock(&lock2);
            break;
        default:
            break;

        }
    }
    
    pthread_exit(0);
    
}

void switchPartsParallel(struct arguments* args){

    if(step == 1)
    {
        if(can_switch.p0 == 1 && can_switch.p2 == 1)
        {
            args->p0.size = parts.p0.lower_size + parts.p2.lower_size;
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
           
            args->p2.size = parts.p2.higher_size + parts.p0.higher_size;
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
        }

        if(can_switch.p1 == 1 && can_switch.p3 == 1)
        {
            args->p1.size = parts.p1.lower_size + parts.p3.lower_size;
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
            
            args->p3.size = parts.p3.higher_size + parts.p1.higher_size;
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
        }

        args->p0.array = &final_array[0];
        args->p2.array = &final_array[args->p1.size + args->p0.size];
        args->p1.array = &final_array[args->p0.size];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];

    } else
    {
        if(can_switch.p0 == 2 && can_switch.p1 == 2)
        {
            args->p0.size = parts.p0.lower_size + parts.p1.lower_size;
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
            quicksort(0,args->p0.size - 1, p_tmp0);
            
            args->p1.size = parts.p1.higher_size + parts.p0.higher_size;
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
            quicksort(0,args->p1.size - 1, p_tmp1);
        }

        if(can_switch.p2 == 2 && can_switch.p3 == 2)
        {
            args->p2.size = parts.p2.lower_size + parts.p3.lower_size;
            int i;
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
            quicksort(0,args->p2.size - 1, p_tmp2);  

            args->p3.size = parts.p3.higher_size + parts.p2.higher_size;
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
            quicksort(0,args->p3.size - 1, p_tmp3); 

        }
        args->p0.array = &final_array[0];
        args->p1.array = &final_array[args->p0.size ];
        args->p2.array = &final_array[args->p1.size + args->p0.size];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
    }

}

void merge(int* final_array, int size)
{
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
  
    result.higher_part = &array[higher_index];
    result.higher_size = size - higher_index;
    result.lower_part = &array[0];
    result.lower_size = higher_index;
    
    return result;
}
struct arguments divideArray(int* array, int size){
    struct arguments result;
    int part_size = size /4;
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