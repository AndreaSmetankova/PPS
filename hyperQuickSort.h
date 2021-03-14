#ifndef HYPERQUICKSORT
#define HYPERQUICKSORT

#define MAXSIZE 1000


struct proces{
    int* array;
    int size;;
    int proces_id;
};

struct arguments {
    struct proces p0;
    struct proces p1;
    struct proces p2;
    struct proces p3;
    
};

struct median_division {
    int* lower_part;
    int lower_size;
    int* higher_part;
    int higher_size;
};

struct changable_parts{
    struct median_division p0;
    struct median_division p1;
    struct median_division p2;
    struct median_division p3;
};

void parallelQuickSort(int* array, int size);
struct arguments divideArray(int* array, int size);
void hyperQuickSort(void* arguments);
struct median_division medianDivision (int* array, int size, int median);
void switchParts(struct arguments* args, int * arrray, int size);
int copyArray(int* source_array, int* destination_array, int size, int start);



#endif