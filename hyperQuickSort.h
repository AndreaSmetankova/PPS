#ifndef HYPERQUICKSORT
#define HYPERQUICKSORT

#define MAXSIZE 1000

struct arrays {
    int p0_array[MAXSIZE];
    int p0_size;
    int p1_array[MAXSIZE];
    int p1_size;
    int p2_array[MAXSIZE];
    int p2_size;
    int p3_array[MAXSIZE];
    int p3_size;
};

struct median_division {
    int lower_part[MAXSIZE];
    int lower_size;
    int higher_part[MAXSIZE];
    int higher_size;
};

struct arguments{
    int* array;
    int size;;
    int proces_id;
};
struct changable_parts{
    struct median_division p0;
    struct median_division p1;
    struct median_division p2;
    struct median_division p3;
};

struct arrays divideArray(int* array, int size);
int copyArray(int* source_array, int* destination_array, int size, int start);
struct median_division medianDivision (int* array, int size, int median);
void hyperQuickSort(void* arguments);
void switchParts (int proces_id);
void parallelQuickSort(int* array, int size);
void mergeArrays();
void printParts();




#endif