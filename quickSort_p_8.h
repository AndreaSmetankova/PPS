#ifndef QUICKSORT_P_8
#define QUICKSORT_P_8
#include "functions.h"

void parallelQuickSort_8threads(int* array, int size);
void handle_8_threads(void* arguments);
void switch8PartsParallel(struct arguments* args,int proces_id);
void merge8(int* final_array, int size);
struct arguments divideArray8parts(int* array, int size);



#endif