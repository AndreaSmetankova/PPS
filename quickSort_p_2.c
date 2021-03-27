#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include "quickSort.h"
#include "quickSort_p_2.h"
#include "functions.h"

/****************************GLOBAL VARIABLES**************************************/

//conditions and mutexes shared between threads
pthread_mutex_t _lock_median1; 
pthread_cond_t _cond_median1; 
pthread_mutex_t _lock_1; 
pthread_cond_t _done_1;

int _median1;
unsigned int _median1_ready = 0; // if 1 median1 has been chanded
struct changable_parts _parts; // structure that saves pointers to array after the array was divided with median
struct arguments _arg; //structure that holds arguments for thread function
int* _p_tmp0 = NULL; //temporary help array 
int* _p_tmp1 = NULL; //temporary help array 
int* _final_array; //pointer to array wchich is being sorted
struct can_switch _can_switch; //if 0 thread 1 must wait for thread 2

/**********************************************************************************/

/*----------------------------------------------
 input parameters: int* array, int size
 function:         create threads and execute algorithm
 returns:          nothing
----------------------------------------------*/
void parallelQuickSort_2threads(int* array, int size){
   pthread_t thread_id0,thread_id1;
   //initialize conditions and mutexes
   pthread_cond_init(&_cond_median1, NULL);
   pthread_mutex_init(&_lock_median1,NULL);
   pthread_cond_init(&_done_1, NULL);
   pthread_mutex_init(&_lock_1, NULL);
    
    //divide array
   _arg = divideArray2parts(array, size);
   _final_array = array;

    //create threads
   pthread_create(&thread_id0,NULL,(void*) handle_2_threads, (void*)&_arg.p0);
   pthread_create(&thread_id1,NULL,(void*) handle_2_threads, (void*)&_arg.p1);
        
   //postpone main thread(the one that created the other threads) until threads are done     
   pthread_join(thread_id0,NULL);
   pthread_join(thread_id1,NULL);


    //merge two sorted parts
   merge2(array,size);
   
   //free alocated memory
   free( _p_tmp0);
   free( _p_tmp1);
    
    
}


/****************************LOCAL FUNCTIONS**************************************/
/*----------------------------------------------
 input parameters: void* arguments
 function:         calls local functions in thread to execute algorithm and manage threads
 returns:          nothing
----------------------------------------------*/
void handle_2_threads(void* arguments){
    struct proces* args = (struct proces *) arguments;
    quicksort(0, args->size - 1, args->array);

    //execute branch acording which thread is called
    if(args->proces_id == 0)
    {
       //lock other threads until median is done
       pthread_mutex_lock(&_lock_median1);
       _median1 =args->array[(args->size - 1)/2];
       _median1_ready = 1;
       _parts.p0 = medianDivision (args->array, args->size, _median1);
       //broadcast to all threads that median1 is ready
       pthread_cond_broadcast(&_cond_median1);
       pthread_mutex_unlock(&_lock_median1); 

        //lock until thread 1 is done
       pthread_mutex_lock(&_lock_1);
       while (_can_switch.p0 != 1)
       {
           //wait until conditions is met
            pthread_cond_wait(&_done_1, &_lock_1);
       }
       //switch parts
       switch2PartsParallel(&_arg,args->proces_id);
       pthread_mutex_unlock(&_lock_1);
    }else if(args->proces_id == 1)
    {
        //wait until median is calculated
       pthread_mutex_lock(&_lock_median1);
       while (_median1_ready != 1)
       {
          pthread_cond_wait(&_cond_median1, &_lock_median1);
       }
       _parts.p1 = medianDivision (args->array, args->size, _median1);
       _can_switch.p0 = 1;
       //signal median division ended - can switch parts
       pthread_cond_signal(&_done_1);
       pthread_mutex_unlock(&_lock_median1);
            
            
    }

    pthread_exit(0);
}


/*----------------------------------------------
 input parameters: struct arguments* args,int proces_id
 function:         switch parts of array divided by median and update arguments for function in threads
 returns:          nothing
----------------------------------------------*/
void switch2PartsParallel(struct arguments* args,int proces_id){

    //update new size of first part of array(p0) for thread 0
    args->p0.size = _parts.p0.lower_size + _parts.p1.lower_size;
    _p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
    int i = 0;
    //copy lower part of array p0 and p1 to temporary array p_tmp0
    while (  i < _parts.p0.lower_size)
    {
        _p_tmp0[i] = _parts.p0.lower_part[i];
        i++;
    }
    for (int j = 0; j < _parts.p1.lower_size; j++)
    {
        _p_tmp0[i] = _parts.p1.lower_part[j];
        i++;
    }
    //sort temporary array
    quicksort(0,args->p0.size - 1, _p_tmp0);

    //update new size of second part of array(p1) for thread 1
    args->p1.size = _parts.p1.higher_size + _parts.p0.higher_size;
    _p_tmp1 = (int*)calloc(args->p1.size, sizeof(int));
    //copy higher part of array p0 and p1 to temporary array p_tmp1
    for ( i = 0; i < _parts.p1.higher_size ; i++)
    {
        _p_tmp1[i] = _parts.p1.higher_part[i];
    }
    for (int j = 0; j < _parts.p0.higher_size; j++)
    {
        _p_tmp1[i] = _parts.p0.higher_part[j];
        i++;
    } 

    //sort temporary array
    quicksort(0,args->p1.size - 1, _p_tmp1);

    //update pointers
    args->p0.array = &_final_array[0];
    args->p1.array = &_final_array[args->p0.size];
}

/*----------------------------------------------
 input parameters: int* final_array, int size
 function:         merge two tempoary arrays back to original array
 returns:          nothing
----------------------------------------------*/
void merge2(int* final_array, int size)
{
   
    int start = 0;
    while( start < _arg.p0.size)
    {
        _final_array[start] = _p_tmp0[start];
        start++;
    }


    for(int i = 0; i < _arg.p1.size; i++)
    {
        _final_array[start] = _p_tmp1[i];
        start++;
    }


}

/*----------------------------------------------
 input parameters: int* array, int size
 function:         divide array with size into 2 parts
 returns:          structure arguments
----------------------------------------------*/
struct arguments divideArray2parts(int* array, int size){
    struct arguments result;
    int part_size = size /2;
    result.p0.array = &array[0];
    result.p0.size = part_size;
    result.p0.proces_id = 0;
    result.p1.array = &array[part_size];
    result.p1.size = size - part_size;
    result.p1.proces_id = 1;

    return result;
    
}

/*********************************************************************************/

