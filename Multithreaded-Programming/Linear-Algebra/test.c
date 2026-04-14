#include <stdio.h>
#include "linear_algebra.h"

void test_vec()
{
    vec_t *vec1;
    vec_t *vec2;

    la_vec_create(&vec1, 10);
    la_vec_create(&vec2, 10);

    double arr1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double arr2[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    la_vec_fill(vec1, arr1, 10);
    la_vec_fill(vec2, arr2, 10);

    vec_t *vec3;
    la_vec_create(&vec3, 10);
    la_vec_add(vec3, vec1, vec2);

    la_vec_display(vec1);
    printf(" + ");
    la_vec_display(vec2);
    printf(" = ");
    la_vec_display(vec3);
    printf("\n");

    la_vec_free(vec1);
    la_vec_free(vec2);
    la_vec_free(vec3);
}

void test_mat()
{
    mat_t *mat1;
    mat_t *mat2;

    la_mat_create(&mat1, 2, 2);
    la_mat_create(&mat2, 2, 2);

    /*
        arr1
            [1 2]
            [3 4]
        arr2
            [5 6]
            [7 8]
    */
    double arr1[4] = {1, 2, 3, 4};
    double arr2[4] = {5, 6, 7, 8};

    la_mat_fill(mat1, arr1, 2, 2);
    la_mat_fill(mat2, arr2, 2, 2);

    mat_t *mat3;
    la_mat_create(&mat3, 2, 2);
    la_mat_add(mat3, mat1, mat2);

    la_mat_display(mat3);
    printf("\n");
}

int main()
{

    test_mat();
    return 0;
}

// gcc test.c linear_algebra.c -o test