#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "quickSort.h"
#include "hyperQuickSort.h"

pthread_mutex_t lock;
pthread_cond_t cond;
int step = 1;
unsigned int done;
int median1,median2;
unsigned int ready = 0;
unsigned int ready2 = 0;
struct changable_parts parts;
struct arguments arg;

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
        
        switchParts(&arg, array, size);

        step++;
        
    }

    

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
            pthread_mutex_unlock(&lock);
            break;
        case 1:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            
            break;
        case 2:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p2 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            break;
        case 3:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
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
            median1 =args->array[(args->size - 1)/2];
            parts.p0 = medianDivision (args->array, args->size, median1);
            ready = 1;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
            
        case 1:
            pthread_mutex_lock(&lock);
            while (ready != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock);
            break;
        case 2:
            pthread_mutex_lock(&lock);
            median2 =args->array[(args->size - 1)/2];
            ready2 = 1;
            parts.p2 = medianDivision (args->array, args->size, median2);
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
            break;
        case 3:
            pthread_mutex_lock(&lock);
            while (ready2 != 1)
            {
               pthread_cond_wait(&cond, &lock);
            }
            parts.p3 = medianDivision (args->array, args->size, median2);
            pthread_mutex_unlock(&lock);
            break;
        default:
            break;

        }
    }
    
    
}

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
