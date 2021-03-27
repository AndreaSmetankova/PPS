#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include <Windows.h>
#include "quickSort_p_8.h"
#include "quickSort.h"
#include "functions.h"

/****************************GLOBAL VARIABLES**************************************/
int step = 1; //variable that is tracking which part of algorithm is being executed
int median1, median2, median3, median4;
// if 1 median1,median2,median3,median4 has been chanded
unsigned int median1_ready = 0;
unsigned int median2_ready = 0;
unsigned int median3_ready = 0;
unsigned int median4_ready = 0;
struct arguments arg; //structure that holds arguments for thread function
struct can_switch can_switch; //structure that holds variables which tells if thread is ready to swap parts
struct changable_parts parts;  // structure that saves pointers to array after the array was divided with median
//temporary arrays
int* p_tmp0 = NULL;
int* p_tmp1 = NULL;
int* p_tmp2 = NULL;
int* p_tmp3 = NULL;
int* p_tmp4 = NULL;
int* p_tmp5 = NULL;
int* p_tmp6 = NULL;
int* p_tmp7 = NULL;
int* final_array = NULL; //pointer to array wchich is being sorted

//conditions and mutexes shared between threads
pthread_mutex_t lock_median1;
pthread_mutex_t lock_median2;
pthread_cond_t cond_median1;
pthread_cond_t cond_median2;
pthread_mutex_t lock_median3;
pthread_mutex_t lock_median4;
pthread_cond_t cond_median3;
pthread_cond_t cond_median4;
pthread_mutex_t lock_1;
pthread_mutex_t lock_2;
pthread_mutex_t lock_3;
pthread_mutex_t lock_4;
pthread_cond_t done_1;
pthread_cond_t done_2;
pthread_cond_t done_3;
pthread_cond_t done_4;
/**********************************************************************************/

/*----------------------------------------------
 input parameters: int* array, int size
 function:         create threads and execute algorithm
 returns:          nothing
----------------------------------------------*/
void parallelQuickSort_8threads(int* array, int size){
    pthread_t thread_id0,thread_id1,thread_id2,thread_id3,thread_id4,thread_id5,thread_id6,thread_id7;
    //initialize conditions and mutexes
    pthread_cond_init(&cond_median1, NULL);
    pthread_cond_init(&cond_median2, NULL);
    pthread_cond_init(&cond_median3, NULL);
    pthread_cond_init(&cond_median4, NULL);
    pthread_mutex_init(&lock_median1,NULL);
    pthread_mutex_init(&lock_median2, NULL);
    pthread_mutex_init(&lock_median3, NULL);
    pthread_mutex_init(&lock_median4, NULL);
    pthread_cond_init(&done_1, NULL);
    pthread_cond_init(&done_2, NULL);
    pthread_cond_init(&done_3, NULL);
    pthread_cond_init(&done_4, NULL);
    pthread_mutex_init(&lock_1, NULL);
    pthread_mutex_init(&lock_3, NULL);
    pthread_mutex_init(&lock_2, NULL);
    pthread_mutex_init(&lock_4, NULL);
    
    //divide array
    arg = divideArray8parts(array, size);
    final_array = array;

    can_switch.p0 = 0;
    can_switch.p1 = 0;
    can_switch.p2 = 0;
    can_switch.p3 = 0;
    can_switch.p4 = 0;
    can_switch.p5 = 0;
    can_switch.p6 = 0;
    can_switch.p7 = 0;

    for(int i = 0; i < 3; i++)
    {

        //create threads
        pthread_create(&thread_id0,NULL,(void*)handle_8_threads, (void*)&arg.p0);
        pthread_create(&thread_id1,NULL,(void*)handle_8_threads, (void*)&arg.p1);
        pthread_create(&thread_id2,NULL,(void*)handle_8_threads, (void*)&arg.p2);
        pthread_create(&thread_id3,NULL,(void*)handle_8_threads, (void*)&arg.p3);
        pthread_create(&thread_id4,NULL,(void*)handle_8_threads, (void*)&arg.p4);
        pthread_create(&thread_id5,NULL,(void*)handle_8_threads, (void*)&arg.p5);
        pthread_create(&thread_id6,NULL,(void*)handle_8_threads, (void*)&arg.p6);
        pthread_create(&thread_id7,NULL,(void*)handle_8_threads, (void*)&arg.p7);

        //postpone main thread(the one that created the other threads) until threads are done     
        pthread_join(thread_id0,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        pthread_join(thread_id4,NULL);
        pthread_join(thread_id5,NULL);
        pthread_join(thread_id6,NULL);
        pthread_join(thread_id7,NULL);

        //merge four sorted parts
        merge8(array,size);

        step++;

    }

    //free alocated memory
    free( p_tmp0);
    free( p_tmp1);
    free( p_tmp2);
    free( p_tmp3);
    free( p_tmp4);
    free( p_tmp5);
    free( p_tmp6);
    free( p_tmp7);


}

/****************************LOCAL FUNCTIONS**************************************/

/*----------------------------------------------
 input parameters: void* arguments
 function:         calls local functions in thread to execute algorithm and manage threads
 returns:          nothing
----------------------------------------------*/
void handle_8_threads(void* arguments){
    struct proces* args = (struct proces *) arguments;
    quicksort(0, args->size - 1, args->array);
    
    //first part of algorithm
    if(step == 1)
    {
        //execute branch acording which thread is called
        switch (args->proces_id)
        {
        case 0:
            //lock other threads until median is done
            pthread_mutex_lock(&lock_median1);
            median1 =args->array[(args->size - 1)/2];
            median1_ready = 1;
            parts.p0 = medianDivision (args->array, args->size, median1);
            //broadcast to all threads that median1 is ready
            pthread_cond_broadcast(&cond_median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 7 is done
            pthread_mutex_lock(&lock_1);
            while (can_switch.p7 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_1, &lock_1);
            }
            //switch parts with p7
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_1);
            break;
        case 1:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 6 is done
            pthread_mutex_lock(&lock_2);
            while (can_switch.p6 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_2, &lock_2);
            }
            //switch parts with p6
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_2);
            break;
        case 2:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p2 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 5 is done
            pthread_mutex_lock(&lock_3);
            while (can_switch.p5 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_3, &lock_3);
            }
            //switch parts with p5
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_3);
            break;
        case 3:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p3 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 4 is done
            pthread_mutex_lock(&lock_4);
            while (can_switch.p4 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_4, &lock_4);
            }
            //switch parts with p4
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_4);
            break;
        case 4:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p4 = medianDivision (args->array, args->size, median1);
            can_switch.p4 = 1;
            //signal median division ended - can switch parts with p3
            pthread_cond_signal(&done_4);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 5:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p5 = medianDivision (args->array, args->size, median1);
            can_switch.p5 = 1;
            //signal median division ended - can switch parts with p2
            pthread_cond_signal(&done_3);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 6:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p6 = medianDivision (args->array, args->size, median1);
            can_switch.p6 = 1;
            //signal median division ended - can switch parts with p1
            pthread_cond_signal(&done_2);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 7:
            //wait until median is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p7 = medianDivision (args->array, args->size, median1);
            can_switch.p7 = 1;
            //signal median division ended - can switch parts with p0
            pthread_cond_signal(&done_1);
            pthread_mutex_unlock(&lock_median1);
            break;
        default:
            break;
        }
    //second part of algorithm
    }else if(step == 2)
    {
        //execute branch acording which thread is called
        switch (args->proces_id)
        {
        case 0:
            //lock  threads that wait for median1 until median1 is done
            pthread_mutex_lock(&lock_median1);
            median1 =args->array[(args->size - 1)/2];
            median1_ready = 1;
            parts.p0 = medianDivision (args->array, args->size, median1);
            //broadcast to all  threads that median1 is ready
            pthread_cond_broadcast(&cond_median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 3 is done
            pthread_mutex_lock(&lock_1);
            while (can_switch.p3 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_1, &lock_1);
            }
            //switch parts with p3
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_1);
            break;
        case 1:
            //wait until median1 is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            pthread_mutex_unlock(&lock_median1);
            
            Sleep(1);
            //lock until thread 2 is done
            pthread_mutex_lock(&lock_2);
            while (can_switch.p2 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_2, &lock_2);
            }
            //switch parts with p2
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_2);
            break;
        case 2:
            //wait until median1 is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p2 = medianDivision (args->array, args->size, median1);
            can_switch.p2 = 1;
            //signal median division ended - can switch parts with p1
            pthread_cond_signal(&done_2);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 3:
            //wait until median1 is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p3 = medianDivision (args->array, args->size, median1);
            can_switch.p3 = 1;
            //signal median division ended - can switch parts with p0
            pthread_cond_signal(&done_1);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 4:
            //lock  threads that wait for median2 until median2 is done
            pthread_mutex_lock(&lock_median2);
            median2 = args->array[(args->size - 1)/2];
            median2_ready = 1;
            parts.p4 = medianDivision (args->array, args->size, median2);
            //broadcast to all  threads that median2 is ready
            pthread_cond_broadcast(&cond_median2);
            pthread_mutex_unlock(&lock_median2);

            Sleep(1);
            //lock until thread 7 is done
            pthread_mutex_lock(&lock_3);
            while (can_switch.p7 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_3, &lock_3);
            }
            //switch parts with p7
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_3);
            break;
        case 5:
            //wait until median2 is calculated
            pthread_mutex_lock(&lock_median2);
            while (median2_ready != 1)
            {
                pthread_cond_wait(&cond_median2, &lock_median2);
            }
            parts.p5 = medianDivision (args->array, args->size, median2);
            pthread_mutex_unlock(&lock_median2);

            Sleep(1);
            //lock until thread 6 is done
            pthread_mutex_lock(&lock_4);
            while (can_switch.p6 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_4, &lock_4);
            }
            //switch parts with p6
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_4);
            break;
        case 6:
            //wait until median2 is calculated
            pthread_mutex_lock(&lock_median2);
            while (median2_ready != 1)
            {
                pthread_cond_wait(&cond_median2, &lock_median2);
            }
            parts.p6 = medianDivision (args->array, args->size, median2);
            can_switch.p6 = 1;
            //signal median division ended - can switch parts with p5
            pthread_cond_signal(&done_4);
            pthread_mutex_unlock(&lock_median2);
            break;
        case 7:
            //wait until median2 is calculated
            pthread_mutex_lock(&lock_median2);
            while (median2_ready != 1)
            {
                pthread_cond_wait(&cond_median2, &lock_median2);
            }
            parts.p7 = medianDivision (args->array, args->size, median2);
            can_switch.p7 = 1;
            //signal median division ended - can switch parts with p4
            pthread_cond_signal(&done_3);
            pthread_mutex_unlock(&lock_median2);
            break;
        
        default:
            break;
        }
    //third part of algorithm
    }else if(step == 3)
    {
        //execute branch acording which thread is called
        switch (args->proces_id)
        {
        case 0:
            //lock  threads that wait for median1 until median1 is done
            pthread_mutex_lock(&lock_median1);
            median1 =args->array[(args->size - 1)/2];
            median1_ready = 1;
            parts.p0 = medianDivision (args->array, args->size, median1);
            //broadcast to all  threads that median1 is ready
            pthread_cond_broadcast(&cond_median1);
            pthread_mutex_unlock(&lock_median1);

            Sleep(1);
            //lock until thread 1 is done
            pthread_mutex_lock(&lock_1);
            while (can_switch.p1 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_1, &lock_1);
            }
            //switch parts with p1
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_1);
            break;
        case 1:
            //wait until median1 is calculated
            pthread_mutex_lock(&lock_median1);
            while (median1_ready != 1)
            {
                pthread_cond_wait(&cond_median1, &lock_median1);
            }
            parts.p1 = medianDivision (args->array, args->size, median1);
            can_switch.p1 = 1;
            //signal median division ended - can switch parts with p0
            pthread_cond_signal(&done_1);
            pthread_mutex_unlock(&lock_median1);
            break;
        case 2:
            //lock  threads that wait for median2 until median2 is done
            pthread_mutex_lock(&lock_median2);
            median2 =args->array[(args->size - 1)/2];
            median2_ready = 1;
            parts.p2 = medianDivision (args->array, args->size, median2);
            //broadcast to all  threads that median2 is ready
            pthread_cond_broadcast(&cond_median2);
            pthread_mutex_unlock(&lock_median2);

            Sleep(1);
             //lock until thread 3 is done
            pthread_mutex_lock(&lock_2);
            while (can_switch.p3 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_2, &lock_2);
            }
            //switch parts with p3
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_2);
            break;
        case 3:
            //wait until median2 is calculated
            pthread_mutex_lock(&lock_median2);
            while (median2_ready != 1)
            {
                pthread_cond_wait(&cond_median2, &lock_median2);
            }
            parts.p3 = medianDivision (args->array, args->size, median2);
            can_switch.p3 = 1;
            //signal median division ended - can switch parts with p2
            pthread_cond_signal(&done_2);
            pthread_mutex_unlock(&lock_median2);
            break;
        case 4:
            //lock  threads that wait for median3 until median3 is done
            pthread_mutex_lock(&lock_median3);
            median3 =args->array[(args->size - 1)/2];
            median3_ready = 1;
            parts.p4 = medianDivision (args->array, args->size, median3);
            //broadcast to all  threads that median3 is ready
            pthread_cond_broadcast(&cond_median3);
            pthread_mutex_unlock(&lock_median3);

            Sleep(1);
            //lock until thread 5 is done
            pthread_mutex_lock(&lock_3);
            while (can_switch.p5 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_3, &lock_3);
            }
            //switch parts with p5
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_3);
            break;
        case 5:
            //wait until median3 is calculated
            pthread_mutex_lock(&lock_median3);
            while (median3_ready != 1)
            {
                pthread_cond_wait(&cond_median3, &lock_median3);
            }
            parts.p5 = medianDivision (args->array, args->size, median3);
            can_switch.p5 = 1;
            //signal median division ended - can switch parts with p4
            pthread_cond_signal(&done_2);
            pthread_mutex_unlock(&lock_median3);
            break;
        case 6:
            //lock  threads that wait for median4 until median4 is done
            pthread_mutex_lock(&lock_median4);
            median4 =args->array[(args->size - 1)/2];
            median4_ready = 1;
            parts.p6 = medianDivision (args->array, args->size, median4);
            //broadcast to all  threads that median4 is ready
            pthread_cond_broadcast(&cond_median4);
            pthread_mutex_unlock(&lock_median4);

            Sleep(1);
            //lock until thread 7 is done
            pthread_mutex_lock(&lock_4);
            while (can_switch.p7 != 1)
            {
                //wait until conditions is met
                pthread_cond_wait(&done_4, &lock_4);
            }
            //switch parts with p7
            switch8PartsParallel(&arg,args->proces_id);
            pthread_mutex_unlock(&lock_4);
            break;
        case 7:
            //wait until median4 is calculated
            pthread_mutex_lock(&lock_median4);
            while (median4_ready != 1)
            {
                pthread_cond_wait(&cond_median4, &lock_median4);
            }
            parts.p7 = medianDivision (args->array, args->size, median4);
            can_switch.p7 = 1;
            //signal median division ended - can switch parts with p6
            pthread_cond_signal(&done_4);
            pthread_mutex_unlock(&lock_median4);
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
void switch8PartsParallel(struct arguments* args,int proces_id){
    int i = 0;
    if(step == 1)
    {
      switch (proces_id)
      {
      //thread 0 - swap parts thread p0 and p7
      case 0:
        
        args->p0.size = parts.p0.lower_size + parts.p7.lower_size;
        p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
        i = 0;
        while (  i < parts.p0.lower_size)
        {
            p_tmp0[i] = parts.p0.lower_part[i];
            i++;
        }
        for (int j = 0; j < parts.p7.lower_size; j++)
        {
            p_tmp0[i] = parts.p7.lower_part[j];
            i++;
        }

        args->p7.size = parts.p7.higher_size + parts.p0.higher_size;
        p_tmp7 = (int*)calloc(args->p7.size, sizeof(int));
        for ( i = 0; i < parts.p7.higher_size ; i++)
        {
            p_tmp7[i] = parts.p7.higher_part[i];
        }
        for (int j = 0; j < parts.p0.higher_size; j++)
        {
            p_tmp7[i] = parts.p0.higher_part[j];
            i++;
        }
        break;
     //thread 1 - swap parts thread p1 and p6
     case 1:
        args->p1.size = parts.p1.lower_size + parts.p6.lower_size;
        p_tmp1 = (int*)calloc(args->p1.size, sizeof(int));
        i = 0;
        while (  i < parts.p1.lower_size)
        {
            p_tmp1[i] = parts.p1.lower_part[i];
            i++;
        }
        for (int j = 0; j < parts.p6.lower_size; j++)
        {
            p_tmp1[i] = parts.p6.lower_part[j];
            i++;
        }

        args->p6.size = parts.p6.higher_size + parts.p1.higher_size;
        p_tmp6 = (int*)calloc(args->p6.size, sizeof(int));
        for ( i = 0; i < parts.p6.higher_size ; i++)
        {
            p_tmp6[i] = parts.p6.higher_part[i];
        }
        for (int j = 0; j < parts.p1.higher_size; j++)
        {
            p_tmp6[i] = parts.p1.higher_part[j];
            i++;
        }
        break;
    //thread 2 - swap parts thread p2 and p5
    case 2:
        args->p2.size = parts.p2.lower_size + parts.p5.lower_size;
        p_tmp2 = (int*)calloc(args->p2.size, sizeof(int));
        i = 0;
        while (  i < parts.p2.lower_size)
        {
            p_tmp2[i] = parts.p2.lower_part[i];
            i++;
        }
        for (int j = 0; j < parts.p5.lower_size; j++)
        {
            p_tmp2[i] = parts.p5.lower_part[j];
            i++;
        }

        args->p5.size = parts.p5.higher_size + parts.p2.higher_size;
        p_tmp5 = (int*)calloc(args->p5.size, sizeof(int));
        for ( i = 0; i < parts.p5.higher_size ; i++)
        {
            p_tmp5[i] = parts.p5.higher_part[i];
        }
        for (int j = 0; j < parts.p2.higher_size; j++)
        {
            p_tmp5[i] = parts.p2.higher_part[j];
            i++;
        }
        break;
    //thread 3 - swap parts thread p3 and p4
    case 3:
        args->p3.size = parts.p3.lower_size + parts.p4.lower_size;
        p_tmp3 = (int*)calloc(args->p3.size, sizeof(int));
        i = 0;
        while (  i < parts.p3.lower_size)
        {
            p_tmp3[i] = parts.p3.lower_part[i];
            i++;
        }
        for (int j = 0; j < parts.p4.lower_size; j++)
        {
            p_tmp3[i] = parts.p4.lower_part[j];
            i++;
        }


        args->p4.size = parts.p4.higher_size + parts.p3.higher_size;
        p_tmp4 = (int*)calloc(args->p4.size, sizeof(int));
        for ( i = 0; i < parts.p4.higher_size ; i++)
        {
            p_tmp4[i] = parts.p4.higher_part[i];
        }
        for (int j = 0; j < parts.p3.higher_size; j++)
        {
            p_tmp4[i] = parts.p3.higher_part[j];
            i++;
        }
        break;
      
      default:
          break;
      }  
      //update pointers
      args->p0.array = &final_array[0];
      args->p2.array = &final_array[args->p1.size + args->p0.size];
      args->p1.array = &final_array[args->p0.size];
      args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
      args->p4.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size];
      args->p5.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size];
      args->p6.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size];
      args->p7.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size + args->p6.size];
    }else if(step == 2)
    {
        switch (proces_id)
        {
        //thread 0 - swap parts thread p0 and p3
        case 0:
            args->p0.size = parts.p0.lower_size + parts.p3.lower_size;
            p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            i = 0;
            while (  i < parts.p0.lower_size)
            {
                p_tmp0[i] = parts.p0.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p3.lower_size; j++)
            {
                p_tmp0[i] = parts.p3.lower_part[j];
                i++;
            }

            args->p3.size = parts.p3.higher_size + parts.p0.higher_size;
            p_tmp3 = (int*)calloc(args->p3.size, sizeof(int));
            for ( i = 0; i < parts.p3.higher_size ; i++)
            {
                p_tmp3[i] = parts.p3.higher_part[i];
            }
            for (int j = 0; j < parts.p0.higher_size; j++)
            {
                p_tmp3[i] = parts.p0.higher_part[j];
                i++;
            }
            break;
        //thread 1 - swap parts thread p1 and p2
        case 1:
            args->p1.size = parts.p1.lower_size + parts.p2.lower_size;
            p_tmp1 = (int*)calloc(args->p1.size, sizeof(int));
            i = 0;
            while (  i < parts.p1.lower_size)
            {
                p_tmp1[i] = parts.p1.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p2.lower_size; j++)
            {
                p_tmp1[i] = parts.p2.lower_part[j];
                i++;
            }

            args->p2.size = parts.p2.higher_size + parts.p1.higher_size;
            p_tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            for ( i = 0; i < parts.p2.higher_size ; i++)
            {
                p_tmp2[i] = parts.p2.higher_part[i];
            }
            for (int j = 0; j < parts.p1.higher_size; j++)
            {
                p_tmp2[i] = parts.p1.higher_part[j];
                i++;
            }
            break;
        //thread 4 - swap parts thread p4 and p7
        case 4:
            args->p4.size = parts.p4.lower_size + parts.p7.lower_size;
            p_tmp4 = (int*)calloc(args->p4.size, sizeof(int));
            i = 0;
            while (  i < parts.p4.lower_size)
            {
                p_tmp4[i] = parts.p4.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p7.lower_size; j++)
            {
                p_tmp4[i] = parts.p7.lower_part[j];
                i++;
            }

            args->p7.size = parts.p7.higher_size + parts.p4.higher_size;
            p_tmp7 = (int*)calloc(args->p7.size, sizeof(int));
            for ( i = 0; i < parts.p7.higher_size ; i++)
            {
                p_tmp7[i] = parts.p7.higher_part[i];
            }
            for (int j = 0; j < parts.p4.higher_size; j++)
            {
                p_tmp7[i] = parts.p4.higher_part[j];
                i++;
            }
            break;
        //thread 5 - swap parts thread p5 and p6
        case 5:
            args->p5.size = parts.p5.lower_size + parts.p6.lower_size;
            p_tmp5 = (int*)calloc(args->p5.size, sizeof(int));
            i = 0;
            while (  i < parts.p5.lower_size)
            {
                p_tmp5[i] = parts.p5.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p6.lower_size; j++)
            {
                p_tmp5[i] = parts.p6.lower_part[j];
                i++;
            }

            args->p6.size = parts.p6.higher_size + parts.p5.higher_size;
            p_tmp6 = (int*)calloc(args->p6.size, sizeof(int));
            for ( i = 0; i < parts.p6.higher_size ; i++)
            {
                p_tmp6[i] = parts.p6.higher_part[i];
            }
            for (int j = 0; j < parts.p5.higher_size; j++)
            {
                p_tmp6[i] = parts.p5.higher_part[j];
                i++;
            }
            break;
        
        default:
            break;
        }
        //update pointers
        args->p0.array = &final_array[0];
        args->p2.array = &final_array[args->p1.size + args->p0.size];
        args->p1.array = &final_array[args->p0.size];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
        args->p4.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size];
        args->p5.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size];
        args->p6.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size];
        args->p7.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size + args->p6.size];
    }else if (step == 3)
    {
        switch (proces_id)
        {
        //thread 0 - swap parts thread p0 and p1
        case 0:
            args->p0.size = parts.p0.lower_size + parts.p1.lower_size;
            p_tmp0 = (int*)calloc(args->p0.size, sizeof(int));
            i = 0;
            while (  i < parts.p0.lower_size)
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
            break;
        //thread 2 - swap parts thread p2 and p3
        case 2:
            args->p2.size = parts.p2.lower_size + parts.p3.lower_size;
            p_tmp2 = (int*)calloc(args->p2.size, sizeof(int));
            i = 0;
            while (  i < parts.p2.lower_size)
            {
                p_tmp2[i] = parts.p2.lower_part[i];
                i++;
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
            break;
        //thread 4 - swap parts thread p4 and p5
        case 4:
            args->p4.size = parts.p4.lower_size + parts.p5.lower_size;
            p_tmp4 = (int*)calloc(args->p4.size, sizeof(int));
            i = 0;
            while (  i < parts.p4.lower_size)
            {
                p_tmp4[i] = parts.p4.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p5.lower_size; j++)
            {
                p_tmp4[i] = parts.p5.lower_part[j];
                i++;
            }
            quicksort(0,args->p4.size - 1, p_tmp4);

            args->p5.size = parts.p5.higher_size + parts.p4.higher_size;
            p_tmp5 = (int*)calloc(args->p5.size, sizeof(int));
            for ( i = 0; i < parts.p5.higher_size ; i++)
            {
                p_tmp5[i] = parts.p5.higher_part[i];
            }
            for (int j = 0; j < parts.p4.higher_size; j++)
            {
                p_tmp5[i] = parts.p4.higher_part[j];
                i++;
            }
            quicksort(0,args->p5.size - 1, p_tmp5);
            break;
        //thread 6 - swap parts thread p6 and p7
        case 6:
            args->p6.size = parts.p6.lower_size + parts.p7.lower_size;
            p_tmp6 = (int*)calloc(args->p6.size, sizeof(int));
            i = 0;
            while (  i < parts.p6.lower_size)
            {
                p_tmp6[i] = parts.p6.lower_part[i];
                i++;
            }
            for (int j = 0; j < parts.p7.lower_size; j++)
            {
                p_tmp6[i] = parts.p7.lower_part[j];
                i++;
            }
            quicksort(0,args->p6.size - 1, p_tmp6);

            args->p7.size = parts.p7.higher_size + parts.p6.higher_size;
            p_tmp7 = (int*)calloc(args->p7.size, sizeof(int));
            for ( i = 0; i < parts.p7.higher_size ; i++)
            {
                p_tmp7[i] = parts.p7.higher_part[i];
            }
            for (int j = 0; j < parts.p6.higher_size; j++)
            {
                p_tmp7[i] = parts.p6.higher_part[j];
                i++;
            }
            quicksort(0,args->p7.size - 1, p_tmp7);
            break;
        
        default:
            break;
        }
        //update pointers
        args->p0.array = &final_array[0];
        args->p2.array = &final_array[args->p1.size + args->p0.size];
        args->p1.array = &final_array[args->p0.size];
        args->p3.array = &final_array[args->p2.size + args->p1.size + args->p0.size];
        args->p4.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size];
        args->p5.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size];
        args->p6.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size];
        args->p7.array = &final_array[args->p2.size + args->p1.size + args->p0.size + args->p3.size + args->p4.size + args->p5.size + args->p6.size];
    }
}

/*----------------------------------------------
 input parameters: int* final_array, int size
 function:         merge eight tempoary arrays back to original array
 returns:          nothing
----------------------------------------------*/
void merge8(int* final_array, int size)
{
    // printf("step = %i \n",step);
    // printf("size %i p_tmp0: ",arg.p0.size);
    // printArray(p_tmp0,arg.p0.size);
    // printf("\n");
    // printf("size %i p_tmp1: ",arg.p1.size);
    // printArray(p_tmp1,arg.p1.size);
    // printf("\n");
    // printf("size %i p_tmp2: ",arg.p2.size);
    // printArray(p_tmp2,arg.p2.size);
    // printf("\n");
    // printf("size %i p_tmp3: ",arg.p3.size);
    // printArray(p_tmp3,arg.p3.size);
    // printf("\n");
    // printf("size %i p_tmp4: ",arg.p4.size);
    // printArray(p_tmp4,arg.p4.size);
    // printf("\n");
    // printf("size %i p_tmp5: ",arg.p5.size);
    // printArray(p_tmp5,arg.p5.size);
    // printf("\n");
    // printf("size %i p_tmp6: ",arg.p6.size);
    // printArray(p_tmp6,arg.p6.size);
    // printf("\n");
    // printf("size %i p_tmp7: ",arg.p7.size);
    // printArray(p_tmp7,arg.p7.size);
    // printf("\n");

    int start = 0;
    while( start < arg.p0.size)
    {
        final_array[start] = p_tmp0[start];
        start++;
    }
    // printf("final array part1: ");
    // printArray(final_array,arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p1.size; i++)
    {
        final_array[start] = p_tmp1[i];
        start++;
    }

    // printf("final array part2: ");
    // printArray(final_array,arg.p1.size + arg.p0.size);
    // printf("\n");
    


    for(int i = 0; i < arg.p2.size; i++)
    {
        final_array[start] = p_tmp2[i];
        start++;
    }

    // printf("final array part3: ");
    // printArray(final_array,arg.p2.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p3.size; i++)
    {
        final_array[start] = p_tmp3[i];
        start++;
    }

    // printf("final array part4: ");
    // printArray(final_array,arg.p3.size + arg.p2.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p4.size; i++)
    {
        final_array[start] = p_tmp4[i];
        start++;
    }

    // printf("final array part5: ");
    // printArray(final_array,arg.p4.size + arg.p2.size +arg.p3.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p5.size; i++)
    {
        final_array[start] = p_tmp5[i];
        start++;
    }

    // printf("final array part6: ");
    // printArray(final_array,arg.p5.size + arg.p4.size + arg.p2.size +arg.p3.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p6.size; i++)
    {
        final_array[start] = p_tmp6[i];
        start++;
    }

    // printf("final array part7: ");
    // printArray(final_array,arg.p6.size + arg.p5.size + arg.p4.size + arg.p2.size +arg.p3.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

    for(int i = 0; i < arg.p7.size; i++)
    {
        final_array[start] = p_tmp7[i];
        start++;
    }

    // printf("step %i final array: ",step);
    // printArray(final_array,arg.p7.size + arg.p6.size + arg.p5.size + arg.p4.size + arg.p2.size +arg.p3.size + arg.p1.size + arg.p0.size);
    // printf("\n");
    

}

/*----------------------------------------------
 input parameters: int* array, int size
 function:         divide array with size into 8 parts
 returns:          structure arguments
----------------------------------------------*/
struct arguments divideArray8parts(int* array, int size){
    struct arguments result;
    int part_size = size /8;
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
    result.p3.size = part_size;
    result.p3.proces_id = 3;
    result.p4.array = &array[part_size * 4];
    result.p4.size = part_size;
    result.p4.proces_id = 4;
    result.p5.array = &array[part_size * 5];
    result.p5.size = part_size;
    result.p5.proces_id = 5;
    result.p6.array = &array[part_size * 6];
    result.p6.size = part_size;
    result.p6.proces_id = 6;
    result.p7.array = &array[part_size * 7];
    result.p7.size = size - 7 * part_size;
    result.p7.proces_id = 7;

    return result;
    
}

/*********************************************************************************/

