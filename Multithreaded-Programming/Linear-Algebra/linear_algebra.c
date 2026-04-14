#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "linear_algebra.h"

/*=======================
    Vector Operations
=======================*/

la_status_t la_vec_create(vec_t **out, size_t size)
{
    vec_t *vec = malloc(sizeof(vec_t));
    if (vec == NULL)
    {
        *out = NULL;
        return LA_ALLOC_FAILED; // User will handle the error
    }

    vec->len = size;
    vec->data = calloc(size, sizeof(vec_data_t));
    if (vec->data == NULL)
    {
        *out = NULL;
        free(vec);
        return LA_ALLOC_FAILED;
    }

    *out = vec;

    return LA_OK;
}

la_status_t la_vec_fill(vec_t *vec, const double *arr, size_t arr_len)
{
    if (vec->len != arr_len)
    {
        return LA_INVALID_DIMENSION;
    }

    memcpy(vec->data, arr, arr_len * sizeof(arr[0]));
    return LA_OK;
}

la_status_t la_vec_clear(vec_t *vec)
{
    memset(vec->data, 0, vec->len *sizeof(vec->data[0]));
    return LA_OK;
}

void la_vec_free(vec_t *vec)
{
    free(vec->data);
    free(vec);
}

void la_vec_display(vec_t *vec)
{
    printf("(");
    for (size_t i = 0; i < vec->len; i++)
    {
        printf("%.2lf", vec->data[i]);
        if (i < vec->len - 1)
        {
            printf(", ");
        }
    }
    printf(")");
}

la_status_t la_vec_add(vec_t *out, const vec_t *vec1, const vec_t *vec2)
{
    if (!(vec_len_eq(out, vec1) && vec_len_eq(out, vec2)))
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t i = 0; i < out->len; i++)
    {
        out->data[i] = vec1->data[i] + vec2->data[i];
    }

    return LA_OK;
}

la_status_t la_vec_sub(vec_t *out, const vec_t *vec1, const vec_t *vec2)
{
    if (out->len != vec1->len || out->len != vec2->len)
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t i = 0; i < out->len; i++)
    {
        out->data[i] = vec1->data[i] - vec2->data[i];
    }

    return LA_OK;
}

la_status_t la_vec_scale(vec_t *out, const vec_t *vec1, double scale)
{
    if (out->len != vec1->len)
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t i = 0; i < out->len; i++)
    {
        out->data[i] = vec1->data[i] * scale;
    }

    return LA_OK;
}

la_status_t la_vec_dot(double *out, const vec_t *vec1, const vec_t *vec2)
{
    *out = 0;

    if (vec1->len != vec2->len)
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t i = 0; i < vec1->len; i++)
    {
        *out += vec1->data[i] * vec2->data[i];
    }

    return LA_OK;
}

/*======================
    Matrix Operations
======================*/

la_status_t la_mat_create(mat_t **out, size_t row, size_t col)
{
    mat_t *mat = malloc(sizeof(mat_t));
    if (mat == NULL)
    {
        *out = NULL;
        return LA_ALLOC_FAILED;
    }

    mat->row = row;
    mat->col = col;
    mat->data = calloc(row * col, sizeof(mat_data_t));
    if (mat->data == NULL)
    {
        *out = NULL;
        free(mat);
        return LA_ALLOC_FAILED;
    }

    *out = mat;
    return LA_OK;
}

la_status_t la_mat_fill(mat_t *mat, const double *arr, size_t arr_row, size_t arr_col)
{
    if (mat->row != arr_row || mat->col != arr_col)
    {
        return LA_INVALID_DIMENSION;
    }

    memcpy(mat->data, arr, arr_row * arr_col * sizeof(arr[0]));

    return LA_OK;
}

la_status_t la_mat_clear(mat_t *mat)
{
    memset(mat->data, 0, mat->row * mat->col * sizeof(mat_data_t));
    return LA_OK;
}

void la_mat_free(mat_t *mat)
{
    free(mat->data);
    free(mat);
}

void la_mat_display(mat_t *mat)
{
    size_t total_size = mat->row * mat->col;
    int cur_max = mat->data[0];

    for (size_t i = 0; i < total_size; i++)
    {
        if (mat->data[i] > cur_max)
        {
            cur_max = mat->data[i];
        }
    }

    int dot = 1;                // dot takes 1 character
    int percision = 2;          // numbers after decimal places
    int spot = percision + dot; // include numbers before decimal places
    while (cur_max > 0)
    {
        cur_max /= 10;
        spot++;
    }

    printf("Spot: %d\n", spot);

    for (size_t row = 0; row < mat->row; row++)
    {
        printf("(");
        for (size_t col = 0; col < mat->col; col++)
        {
            printf("%*.*lf", spot, percision, mat->data[row * mat->row + col]);
            if (col < mat->col - 1)
            {
                printf(" ");
            }
        }
        printf(")");

        if (row < mat->row - 1)
        {
            printf("\n");
        }
    }
}

la_status_t la_mat_add(mat_t *out, const mat_t *mat1, const mat_t *mat2)
{
    if (!(mat_size_eq(out, mat1) && mat_size_eq(out, mat2)))
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t row = 0; row < out->row; row++)
    {
        for (size_t col = 0; col < out->col; col++)
        {
            size_t idx = row * out->row + col;
            out->data[idx] = mat1->data[idx] + mat2->data[idx];
        }
    }

    return LA_OK;
}

la_status_t la_mat_sub(mat_t *out, const mat_t *mat1, const mat_t *mat2)
{
    if (!(mat_size_eq(out, mat1) && mat_size_eq(out, mat2)))
    {
        return LA_INVALID_DIMENSION;
    }

    for (size_t row = 0; row < out->row; row++)
    {
        for (size_t col = 0; col < out->col; col++)
        {
            size_t idx = row * out->row + col;
            out->data[idx] = mat1->data[idx] - mat2->data[idx];
        }
    }

    return LA_OK;
}

la_status_t la_mat_scale(mat_t *out, const mat_t *mat1, double scale);
la_status_t la_mat_mul(mat_t *out, const mat_t *mat1, const mat_t *mat2, const la_context_t *la_cxt);
la_status_t la_mat_transpose(mat_t *out, const mat_t *mat1);