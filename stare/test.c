#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

void * myThreadFun1(void * arguments);
void * myThreadFun2(void * arguments);
void printArray(int array[], int size);

sem_t mutex;
pthread_cond_t cond;
struct argument_struct{
    int a;
    int b;
    int array_size;
    int* array;
};


int main(){
    struct argument_struct arguments;
    int pole[] = {1,2,3,4};
    int size = sizeof(pole) / sizeof(pole[0]);
    pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
    sem_init(&mutex,0,1);
    arguments.a = 1;
    arguments.b = 2;
    arguments.array = pole;
    arguments.array_size = size;
    
   
    printf("%d\n", size);
    pthread_t thread_id1,thread_id2,thread_id3;
    printf("Before threads\n");
    printf("Array : ");
    printArray(arguments.array, size);
    printf("-----------------------MAIN------------------------------------------------\n");
    pthread_create(&thread_id1,NULL,(void*)myThreadFun1, (void*)&arguments);
    pthread_create(&thread_id2,NULL,(void*)myThreadFun2, (void*)&arguments);
    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);
    printf("After thread\n");
    printf("Array : ");
    printArray(arguments.array, size);
    printf("##################################################################################\n");
    pthread_create(&thread_id3,NULL,(void*)myThreadFun2, (void*)&arguments);
    pthread_join(thread_id3,NULL);
    printf("-----------------------MAIN END------------------------------------------------\n");
    sem_destroy(&mutex);

    return 0;
}

void * myThreadFun1(void * arguments){
    printf("before thread 1\n");
    sem_wait(&mutex);
    printf("-----------------------THREAD 1------------------------------------------------\n");
    sleep(1);
    struct argument_struct *args = (struct argument_struct *)arguments;

    int result = args->a * args->array[1] + args->b;
    args->array[1] = result;
    
    printf("Printing from my thread 1 result = %d\n", result);
    // printf("Array 1: ");
    // printArray(args->array,args->array_size);
    
    printf("-----------------------THREAD 1 END------------------------------------------------\n");
    sem_post(&mutex);
    pthread_cond_signal(&cond);
    return NULL;
}

void * myThreadFun2(void * arguments){
    
    printf("before thread 2\n");
    sleep(1);
    sem_wait(&mutex);
    pthread_cond_wait(&cond, &mutex);
    printf("-----------------------THREAD 2------------------------------------------------\n");
    
    struct argument_struct *args = (struct argument_struct *)arguments;

    // printf("Array 2: ");
    // printArray(args->array,args->array_size);

    int result = args->b * args->array[1] + args->a;
    args->array[2] = result;
    printf("Printing from my thread 2 result = %d\n", result);
    printf("-----------------------THREAD 2 END------------------------------------------------\n");
    sem_post(&mutex);

    return NULL;
}

void printArray(int array[], int size){
    if(array == NULL)
    {
        return;
    }
    for(int i = 0; i < size; i++)
    {
        printf("%d ",array[i]);
    }
    printf("\n");
}