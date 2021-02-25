#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void * myThreadFun(void* vargp){
    sleep(1);
    printf("Printing from my thread\n");
    return NULL;
}

int main(){
    pthread_t thread_id;
    printf("Before thread\n");
    pthread_create(&thread_id,NULL,myThreadFun, NULL);
    pthread_join(thread_id,NULL);
    printf("After thread\n");

    return 0;
}