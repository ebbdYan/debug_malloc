#include <stdlib.h>
#include <stdio.h>

#include "debugmalloc.h"

int main()
{
    int i = 0;
    char *p[10000];

    while(i < 10000)
    {   
        if(i / 2 == 0){
            p[i] = malloc(1);
            printf("%d\n", i);
            free(p[i]);
        }
        else{
            p[i] = calloc(1, sizeof(int));
            printf("%d\n", i);
            if(i == 9999)
                break;
            free(p[i]);
        }
        i++;
    }
    
    //free(p);
    MALLOC_DUMP();

    return 0;
}