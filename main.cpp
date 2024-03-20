#include <stdlib.h>

#include "debugmalloc.h"

int main()
{
    void *p = malloc (1024);

    // free (p);

    MALLOC_DUMP();

    return 0;
}