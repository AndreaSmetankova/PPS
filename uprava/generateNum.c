//ToDO: vygeneruj file s danym poctom cisel
#include <stdio.h>
#include <stdlib.h>
#include<time.h>

void generateNumFile(int size,char* file){
    
    FILE* fp;
    fp = fopen(file, "w+");

    srand(time(0));
 
    for(int i = 0; i < size; i++)
    {
        if(i == (size - 1))
        {
            fprintf("%d",rand(), fp); 
        } else
        {
            fprintf("%d,",rand(), fp);
        }
        
    }
    fclose(fp);    


}