#include <stdio.h>
#include <stdlib.h>

typedef struct Pixel {
    unsigned char R, G, B;
} Pixel;

unsigned char check_file_error_null(FILE * tmp)
{
    if (!tmp)
    {
        printf("\nFile not found\n");
        return 1;
    }
    return 0;
}

unsigned char check_memory_allocation(const void * data)
{
    if (!data)
    {
        printf("Couldn't allocate memory");
        return 1;
    }
    return 0;
}