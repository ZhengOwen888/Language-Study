#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
    Sum all number between 0 and n inclusive.
*/
int sum(int n)
{
    assert(n >= 0);

    int total = 0;

    for (int i = 0; i <= n; i++)
    {
        printf("===========================\n");
        printf("Current Number: %d\n", i);
        total += i;
        printf("Current Total:  %d\n", total);
        printf("===========================\n");
    }

    return total;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./sum integer\n");
        exit(1);
    }
    int num = atoi(argv[1]);

    int *ptr = NULL; // NULL pointer
    printf("About to dereference a pointer");
    *ptr = sum(num); // Segmentation fault occurs
    printf("This line will never be reached");

    return 0;
}