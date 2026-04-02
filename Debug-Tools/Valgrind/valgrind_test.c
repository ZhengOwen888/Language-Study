#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *num = malloc(sizeof(int));
    *num = 9;
    printf("Value: %d\n", *num);

    free(num);

    // Use after free -> Undefined behavior -> Can cause segmentation fault.
    *num = 10;

    // Forgot to free -> memory leak -> program crash
    int *leak = malloc(10 * sizeof(int));

    return 0;
}