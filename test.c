#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>



struct argument_struct{
    int a;
    int b;
    int* array;
};

void * myThreadFun(void * arguments){
    sleep(1);
    struct argument_struct *args = (struct argument_struct *)arguments;
    printf("Printing from my thread %d %d\n",args->a,args->b);

    return NULL;
}

int main(){
    struct argument_struct arguments;
    int pole[] = {1,2,3,4};
    arguments.a = 1;
    arguments.b = 2;
    arguments.array = pole;
    pthread_t thread_id;
    printf("Before thread\n");
    pthread_create(&thread_id,NULL,(void*)myThreadFun, (void*)&arguments);
    pthread_join(thread_id,NULL);
    printf("After thread\n");

    return 0;
}