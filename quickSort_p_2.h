#ifndef QUICKSORT_P_2
#define QUICKSORT_P_2
#include "functions.h"

void parallelQuickSort_2threads(int* array, int size);
void handle_2_threads(void* arguments);
void switch2PartsParallel(struct arguments* args,int proces_id);
void merge2(int* final_array, int size);
struct arguments divideArray2parts(int* array, int size);

#endif