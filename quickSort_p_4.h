#ifndef QUICKSORT_P_4
#define QUICKSORT_P_4
#include "functions.h"

void parallelQuickSort_4threads(int* array, int size);
void handle_4_threads(void* arguments);
void switch4PartsParallel(struct arguments* args,int proces_id);
void merge4(int* final_array, int size);
struct arguments divideArray4parts(int* array, int size);

#endif