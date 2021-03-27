#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

/*----------------------------------------------
 input parameters: int* source_array, int* destination_array, int size, int start
 function:         copy numbers from source array to destionation array from index start until inex size
 returns:          int - how many numbers were copied
----------------------------------------------*/
int copyArray(int* source_array, int* destination_array, int size, int start){
    int j = 0;
    for(int i = start; i < size; i++)
    {
        destination_array[j] = source_array[i];
        j++;
    }
    
    return j ;
}

/*----------------------------------------------
 input parameters: int* array, int size, int median
 function:         divide array with size into 2 parts(lower and higher) with median
 returns:          struct median_division
----------------------------------------------*/
struct median_division medianDivision (int* array, int size, int median){
    struct median_division result;
    int higher_index = 0;

    if(array[0] > median)
    {
        result.higher_size = size;
        result.higher_part = array;
        result.lower_part = NULL;
        result.lower_size = 0;
        return result;

    } 
    else if(array[size - 1] <= median)
    {
        result.higher_size = 0;
        result.higher_part = NULL;
        result.lower_part = array;
        result.lower_size = size;
        return result;
    }

    for(int i = 0; i < size; i++)
    {
        if (array[i] > median)
        {
            higher_index = i;
            break;
        }
    }
    result.higher_part = &array[higher_index];
    result.higher_size = size - higher_index;
    result.lower_part = &array[0];
    result.lower_size = higher_index;
    
    return result;
}