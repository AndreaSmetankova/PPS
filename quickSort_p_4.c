#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "quickSort.h"
#include "quickSort_p_4.h"
#include <Windows.h>
#include "functions.h"

/****************************GLOBAL VARIABLES**************************************/

//conditions and mutexes shared between threads
pthread_mutex_t lock_median1_;
pthread_mutex_t lock_median2_;
pthread_cond_t cond_median1_;
pthread_cond_t cond_median2_;
pthread_mutex_t lock_1;
pthread_mutex_t lock_2;
pthread_mutex_t lock_3;
pthread_cond_t done_1;
pthread_cond_t done_2;
pthread_cond_t done_3;

int step_ = 1; //variable that is tracking which part of algorithm is being executed
int median1_, median2_;
// if 1 median1 and median2 has been chanded
unsigned int median1__ready = 0;
unsigned int median2__ready = 0;

struct can_switch can__switch; //structure that holds variables which tells if thread is ready to swap parts
struct changable_parts parts_; // structure that saves pointers to array after the array was divided with median
struct arguments arg_; //structure that holds arguments for thread function
//temporary arrays
int* p__tmp0 = NULL;
int* p__tmp1 = NULL;
int* p__tmp2 = NULL;
int* p__tmp3 = NULL;
int* final__array; //pointer to array wchich is being sorted
/**********************************************************************************/

/*----------------------------------------------
 input parameters: int* array, int size
 function:         create threads and execute algorithm
 returns:          nothing
----------------------------------------------*/
void parallelQuickSort_4threads(int* array, int size){
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3;
    //initialize conditions and mutexes
    pthread_cond_init(&cond_median1_, NULL);
    pthread_cond_init(&cond_median2_, NULL);
    pthread_mutex_init(&lock_median1_,NULL);
    pthread_mutex_init(&lock_median2_, NULL);
    pthread_cond_init(&done_3, NULL);
    pthread_cond_init(&done_2, NULL);
    pthread_cond_init(&done_1, NULL);
    pthread_mutex_init(&lock_1, NULL);
    pthread_mutex_init(&lock_3, NULL);
    pthread_mutex_init(&lock_2, NULL);
    
    //divide array
    arg_ = divideArray4parts(array, size);
    final__array = array;

    can__switch.p0 = 0;
    can__switch.p1 = 0;
    can__switch.p2 = 0;
    can__switch.p3 = 0;

    for(int i = 0; i < 2; i++)
    {

        //create threads
        pthread_create(&thread_id0,NULL,(void*)handle_4_threads, (void*)&arg_.p0);
        pthread_create(&thread_id1,NULL,(void*)handle_4_threads, (void*)&arg_.p1);
        pthread_create(&thread_id2,NULL,(void*)handle_4_threads, (void*)&arg_.p2);
        pthread_create(&thread_id3,NULL,(void*)handle_4_threads, (void*)&arg_.p3);
        
        //postpone main thread(the one that created the other threads) until threads are done     
        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);

        //merge four sorted parts
        merge4(array,size);

        step_++;
    }

    //free alocated memory
    free( p__tmp0);
    free( p__tmp1);
    free( p__tmp2);
    free( p__tmp3);
}

/****************************LOCAL FUNCTIONS**************************************/

/*----------------------------------------------
 input parameters: void* arguments
 function:         calls local functions in thread to execute algorithm and manage threads
 returns:          nothing
----------------------------------------------*/
void handle_4_threads(void* arguments){
    struct proces* args = (struct proces *) arguments;
    quicksort(0, args->size - 1, args->array);
  
    //first part of algorithm
    if(step_ == 1)
    {
         //execute branch acording which thread is called
        switch(args->proces_id)
        {
        case 0:
            //lock other threads until median is done
            pthread_mutex_lock(&lock_median1_);
            median1_ =args->array[(args->size - 1)/2];
            median1__ready = 1;
            parts_.p0 = medianDivision (args->array, args->size, median1_);
            //broadcast to all threads that median1 is ready
            pthread_cond_broadcast(&cond_median1_);
            pthread_mutex_unlock(&lock_median1_);

            Sleep(1);
             //lock until thread 2 is done
            pthread_mutex_lock(&lock_2);
            while (can__switch.p2 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_2, &lock_2);
            }
            //switch parts with p2
            switch4PartsParallel(&arg_,args->proces_id);
            pthread_mutex_unlock(&lock_2);
            break;
        case 1:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1_);
            while (median1__ready != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&cond_median1_, &lock_median1_);
            }
            parts_.p1 = medianDivision (args->array, args->size, median1_);
            pthread_mutex_unlock(&lock_median1_);
            
            Sleep(1);
            //lock until thread 3 is done
            pthread_mutex_lock(&lock_3);
            while (can__switch.p3 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_3, &lock_3);
            }
            //switch parts with p3
           switch4PartsParallel(&arg_,args->proces_id);
            pthread_mutex_unlock(&lock_3);
            
            break;
        case 2:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1_);
            while (median1__ready != 1)
            {
                pthread_cond_wait(&cond_median1_, &lock_median1_);
            }
            parts_.p2 = medianDivision (args->array, args->size, median1_);
            can__switch.p2 = 1;
            //signal median division ended - can switch parts with p0
            pthread_cond_signal(&done_2);
            pthread_mutex_unlock(&lock_median1_);
            break;
        case 3:
             //wait until median is calculated
            pthread_mutex_lock(&lock_median1_);
            while (median1__ready != 1)
            {
                pthread_cond_wait(&cond_median1_, &lock_median1_);
            }
            parts_.p3 = medianDivision (args->array, args->size, median1_);
            can__switch.p3 = 1;
            //signal median division ended - can switch parts with p1
            pthread_cond_signal(&done_3);
            pthread_mutex_unlock(&lock_median1_);
            break;
       
        default:
            break;

        }

        
    } else
    {
        //second part of algorithm
        switch(args->proces_id)
        {
        case 0:
            //lock  threads that wait for median1 until median1 is done
            pthread_mutex_lock(&lock_median1_);
            median1_ =args->array[(args->size - 1)/2];
            median1__ready = 1;
            parts_.p0 = medianDivision (args->array, args->size, median1_);
            //broadcast to all  threads that median1 is ready
            pthread_cond_broadcast(&cond_median1_);
            pthread_mutex_unlock(&lock_median1_);

            Sleep(1);
            //lock until thread 1 is done
            pthread_mutex_lock(&lock_1);
            while (can__switch.p1 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_1, &lock_1);
            }
            //switch parts with p1
            switch4PartsParallel(&arg_,args->proces_id);
            pthread_mutex_unlock(&lock_1);
            
            break;
            
        case 1:
            //wait until median1 is calculated
            pthread_mutex_lock(&lock_median1_);
            while (median1__ready != 1)
            {
                pthread_cond_wait(&cond_median1_, &lock_median1_);
            }
            parts_.p1 = medianDivision (args->array, args->size, median1_);
            
            can__switch.p1 = 1;
            //signal median division ended - can switch parts with p0
            pthread_cond_signal(&done_1);
            pthread_mutex_unlock(&lock_median1_);
            
            break;
        case 2:
            //lock  threads that wait for median2 until median2 is done
            pthread_mutex_lock(&lock_median2_);
            median2_ =args->array[(args->size - 1)/2];
            median2__ready = 1;
            parts_.p2 = medianDivision (args->array, args->size, median2_);
            //broadcast to all  threads that median2 is ready
            pthread_cond_broadcast(&cond_median2_);
            pthread_mutex_unlock(&lock_median2_);

            Sleep(1);
            //lock until thread 3 is done
            pthread_mutex_lock(&lock_3);
            while (can__switch.p3 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_3, &lock_3);
            }
            //switch parts with p3
            switch4PartsParallel(&arg_,args->proces_id);
            pthread_mutex_unlock(&lock_3);
            break;
        case 3:
            //wait until median2 is calculated
            pthread_mutex_lock(&lock_median2_);
            while (median2__ready != 1)
            {
                pthread_cond_wait(&cond_median2_, &lock_median2_);
            }
            parts_.p3 = medianDivision (args->array, args->size, median2_);

            can__switch.p3 = 1;
            //signal median division ended - can switch parts with p2
            pthread_cond_signal(&done_3);
            pthread_mutex_unlock(&lock_median2_);
            break;
        default:
            break;

        }
    }
    
    pthread_exit(0);
    
}


/*----------------------------------------------
 input parameters: struct arguments* args,int proces_id
 function:         switch parts of array divided by median and update arguments for function in threads
 returns:          nothing
----------------------------------------------*/
void switch4PartsParallel(struct arguments* args, int proces_id){

    if(step_ == 1)
    {
        //thread 0 - swap parts thread p0 and p2
        if( proces_id == 0)
        {
            //update new size of first part of array(p0) for thread 0
            args->p0.size = parts_.p0.lower_size + parts_.p2.lower_size;
            p__tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            int i = 0;
            //copy lower part of array p0 and p2 to temporary array p_tmp0
            while (  i < parts_.p0.lower_size)
            {
                p__tmp0[i] = parts_.p0.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts_.p2.lower_size; j++)
            {
                p__tmp0[i] = parts_.p2.lower_part[j];
                i++;
            }
        
            //update new size of second part of array(p2) for thread 2
            args->p2.size = parts_.p2.higher_size + parts_.p0.higher_size;
            p__tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            //copy higher part of array p0 and p2 to temporary array p_tmp2
            for ( i = 0; i < parts_.p2.higher_size ; i++)
            {
                p__tmp2[i] = parts_.p2.higher_part[i];
            }
            for (int j = 0; j < parts_.p0.higher_size; j++)
            {
                p__tmp2[i] = parts_.p0.higher_part[j];
                i++;
            }
             
        } else  
        //thread 1 - swap parts thread p1 and p3
        if(proces_id == 1)
        {
            //update new size of third part of array(p1) for thread 1
            args->p1.size = parts_.p1.lower_size + parts_.p3.lower_size;
            int i;
            p__tmp1 = (int*)calloc(args->p1.size, sizeof(int));
            //copy lower part of array p1 and p3 to temporary array p_tmp1
            for ( i = 0; i < parts_.p1.lower_size ; i++)
            {
                p__tmp1[i] = parts_.p1.lower_part[i];
            }
            for (int j = 0; j < parts_.p3.lower_size; j++)
            {
                p__tmp1[i] = parts_.p3.lower_part[j];
                i++;
            } 

            //update new size of fourth part of array(p3) for thread 3
            args->p3.size = parts_.p3.higher_size + parts_.p1.higher_size;
            p__tmp3 = (int*)calloc(args->p3.size, sizeof(int));
            //copy higher part of array p3 and p1 to temporary array p_tmp3
            for ( i = 0; i < parts_.p3.higher_size ; i++)
            {
                p__tmp3[i] = parts_.p3.higher_part[i];
            }

            for (int j = 0; j < parts_.p1.higher_size; j++)
            {
                p__tmp3[i] = parts_.p1.higher_part[j];
                i++;
            }
        }

        //update pointers
        args->p0.array = &final__array[0];
        args->p2.array = &final__array[args->p1.size + args->p0.size];
        args->p1.array = &final__array[args->p0.size];
        args->p3.array = &final__array[args->p2.size + args->p1.size + args->p0.size];

    } else
    {
        //thread 0 - swap parts thread p0 and p1
        if( proces_id == 0)
        {
            //update new size of first part of array(p0) for thread 0
            args->p0.size = parts_.p0.lower_size + parts_.p1.lower_size;
            p__tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            int i = 0;
            //copy lower part of array p0 and p1 to temporary array p_tmp0
            while ( i < parts_.p0.lower_size)
            {
                p__tmp0[i] = parts_.p0.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts_.p1.lower_size; j++)
            {
                p__tmp0[i] = parts_.p1.lower_part[j];
                i++;
            }
            quicksort(0,args->p0.size - 1, p__tmp0);

            //update new size of second part of array(p1) for thread 1
            args->p1.size = parts_.p1.higher_size + parts_.p0.higher_size;
            p__tmp1 = (int*)calloc(args->p1.size, sizeof(int));
            //copy higher part of array p0 and p1 to temporary array p_tmp1
            for ( i = 0; i < parts_.p1.higher_size ; i++)
            {
                p__tmp1[i] = parts_.p1.higher_part[i];
            }
            for (int j = 0; j < parts_.p0.higher_size; j++)
            {
                p__tmp1[i] = parts_.p0.higher_part[j];
                i++;
            }
            quicksort(0,args->p1.size - 1, p__tmp1);
        } else
        //thread 2 - swap parts thread p3 and p2
        if( proces_id == 2)
        {
            //update new size of third part of array(p2) for thread 2
            args->p2.size = parts_.p2.lower_size + parts_.p3.lower_size;
            int i;
            p__tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            //copy lower part of array p2 and p3 to temporary array p_tmp2
            for ( i = 0; i < parts_.p2.lower_size ; i++)
            {
                p__tmp2[i] = parts_.p2.lower_part[i];
            }
            for (int j = 0; j < parts_.p3.lower_size; j++)
            {
                p__tmp2[i] = parts_.p3.lower_part[j];
                i++;
            } 
            quicksort(0,args->p2.size - 1, p__tmp2); 

            //update new size of fourth part of array(p3) for thread 3 
            args->p3.size = parts_.p3.higher_size + parts_.p2.higher_size;
            p__tmp3 = (int*)calloc(args->p3.size, sizeof(int));
            //copy higher part of array p2 and p3 to temporary array p_tmp3
            for ( i = 0; i < parts_.p3.higher_size ; i++)
            {
                p__tmp3[i] = parts_.p3.higher_part[i];
            }

            for (int j = 0; j < parts_.p2.higher_size; j++)
            {
                p__tmp3[i] = parts_.p2.higher_part[j];
                i++;
            }
            quicksort(0,args->p3.size - 1, p__tmp3); 
        }

        //update pointers
        args->p0.array = &final__array[0];
        args->p1.array = &final__array[args->p0.size ];
        args->p2.array = &final__array[args->p1.size + args->p0.size];
        args->p3.array = &final__array[args->p2.size + args->p1.size + args->p0.size];
    
    }

}


/*----------------------------------------------
 input parameters: int* final_array, int size
 function:         merge four tempoary arrays back to original array
 returns:          nothing
----------------------------------------------*/
void merge4(int* final_array, int size)
{
    
    int start = 0;
    while( start < arg_.p0.size)
    {
        final__array[start] = p__tmp0[start];
        start++;
    }


    for(int i = 0; i < arg_.p1.size; i++)
    {
        final__array[start] = p__tmp1[i];
        start++;
    }


    for(int i = 0; i < arg_.p2.size; i++)
    {
        final__array[start] = p__tmp2[i];
        start++;
    }

    for(int i = 0; i < arg_.p3.size; i++)
    {
        final__array[start] = p__tmp3[i];
        start++;
    }

}


/*----------------------------------------------
 input parameters: int* array, int size
 function:         divide array with size into 4 parts
 returns:          structure arguments
----------------------------------------------*/
struct arguments divideArray4parts(int* array, int size){
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

/*********************************************************************************/