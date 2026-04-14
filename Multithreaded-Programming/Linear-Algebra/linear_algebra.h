#ifndef MY_LINEAR_ALGEBRA_MULTITHREADED
#define MY_LINEAR_ALGEBRA_MULTITHREADED

#include <sys/types.h>

#define LA_MAX_THREADS 8

// Error Code for debugging
typedef enum la_status
{
    LA_OK = 0,
    LA_INVALID_DIMENSION,
    LA_ALLOC_FAILED
} la_status_t;

// Configuration for Vector and Matrix Operations
typedef struct la_context
{
    size_t num_threads;
    size_t block_size;
} la_context_t;

// Linear Algebra Data Types

typedef double vec_data_t;
typedef double mat_data_t;

typedef struct vec
{
    size_t len;
    vec_data_t *data;
} vec_t;

typedef struct mat
{
    size_t row;
    size_t col;
    mat_data_t *data;
} mat_t;

// Vector Operations
la_status_t la_vec_create(vec_t **out, size_t size);
la_status_t la_vec_fill(vec_t *vec, const double *arr, size_t arr_len);
la_status_t la_vec_clear(vec_t *vec);
void la_vec_free(vec_t *vec);
void la_vec_display(vec_t *vec);

la_status_t la_vec_add(vec_t *out, const vec_t *vec1, const vec_t *vec2);
la_status_t la_vec_sub(vec_t *out, const vec_t *vec1, const vec_t *vec2);
la_status_t la_vec_scale(vec_t *out, const vec_t *vec1, double scale);
la_status_t la_vec_dot(double *out, const vec_t *vec1, const vec_t *vec2);
static inline int vec_len_eq(const vec_t *vec1, const vec_t *vec2)
{
    return (vec1->len == vec2->len) ? 1 : 0;
}

// Matrix Operations
la_status_t la_mat_create(mat_t **out, size_t row, size_t col);
la_status_t la_mat_fill(mat_t *mat, const double *arr, size_t arr_row, size_t arr_col);
la_status_t la_mat_clear(mat_t *mat);
void la_mat_free(mat_t *mat);
void la_mat_display(mat_t *mat);

la_status_t la_mat_add(mat_t *out, const mat_t *mat1, const mat_t *mat2);
la_status_t la_mat_sub(mat_t *out, const mat_t *mat1, const mat_t *mat2);
la_status_t la_mat_scale(mat_t *out, const mat_t *mat1, double scale);
la_status_t la_mat_mul(mat_t *out, const mat_t *mat1, const mat_t *mat2, const la_context_t *la_cxt);
la_status_t la_mat_transpose(mat_t *out, const mat_t *mat1);

static inline int mat_size_eq(const mat_t *mat1, const mat_t *mat2)
{
    return (mat1->row == mat2->row && mat1->col == mat2->col) ? 1 : 0;
}

#endif