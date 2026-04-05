#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"

int cmp_int(const void *a, const void *b)
{
    int elem1 = *(const int *)a;
    int elem2 = *(const int *)b;

    if (elem1 < elem2)
    {
        return -1;
    }
    else if (elem1 > elem2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    DynamicArray *arr = da_create_dynamic_array(sizeof(int), cmp_int);
    if (arr == NULL)
    {
        perror("Failed to create dynamic array: ");
        exit(EXIT_FAILURE);
    }

    // Append 0 - 5
    for (int i = 0; i < 6; i++)
    {
        da_push_back(arr, &i);
    }

    // Append 0 - 5
    for (int i = 0; i < 6; i++)
    {
        da_push_back(arr, &i);
    }

    // Current Array : [0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5]
    for (int i = 0; i < 12; i++)
    {
        int num = *(int *)da_get(arr, i);
        printf("Num %d: %d\n", i + 1, num);
    }

    da_free(arr);
    return 0;
}

// gcc dynamic_array.c test.c -o test