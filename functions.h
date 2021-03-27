#ifndef FUNCTIONS
#define FUNCTIONS
#include <pthread.h>

/*******************STRUCT PROCES***************************
    int* array - pointer to part of array
    int size   - size of part
    int proces_id - id of thread
***********************************************************/
struct proces{
    int* array;
    int size;;
    int proces_id;
};


/*******************STRUCT ARGUMENTS***************************
    struct proces p0 - p7 - arguments for function in threads
***************************************************************/
struct arguments {
    struct proces p0;
    struct proces p1;
    struct proces p2;
    struct proces p3;
    struct proces p4;
    struct proces p5;
    struct proces p6;
    struct proces p7;
    
};

/*******************STRUCT MEDIAN DIVISION***************************
    int* lower_part - lower part of array after median division
    int lower_size - size of lower part
    int* higher_part - higher part of array after median division
    int higher_size - size of higher part
*********************************************************************/
struct median_division {
    int* lower_part;
    int lower_size;
    int* higher_part;
    int higher_size;
};

/*******************STRUCT CHANGABLE PARTS***************************
     struct median_division p0 - p7 - parts that are going to be exanchaned between threads
***************************************************************/
struct changable_parts{
    struct median_division p0;
    struct median_division p1;
    struct median_division p2;
    struct median_division p3;
    struct median_division p4;
    struct median_division p5;
    struct median_division p6;
    struct median_division p7;
};

/*******************STRUCT CAN SWITCH***************************
    int p0 - p7 - variables that tell if thread is ready to exchange parts(0 - NO, 1 - YES)
***************************************************************/
struct can_switch {
    int p0;
    int p1;
    int p2;
    int p3; 
    int p4;
    int p5;
    int p6;
    int p7;
};




struct median_division medianDivision (int* array, int size, int median);
int copyArray(int* source_array, int* destination_array, int size, int start);





#endif