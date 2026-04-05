#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include <sys/types.h>

#define DA_MIN_CAPACITY 2
#define DA_MIN_ELEM_SIZE 1

typedef int (*CompareFunc)(const void *, const void *);

typedef struct dynamic_array
{
    char *arr;
    char *end;
    size_t arr_size;
    size_t min_capacity;
    size_t capacity;
    size_t elem_size;
    CompareFunc cmp_func;
} DynamicArray;

typedef enum da_status
{
    DA_OK = 0,
    DA_ERR_INDEX,
    DA_ERR_VALUE,
    DA_ERR_NULL_ARR,
    DA_ERR_ALLOC,
    DA_ERR_EMPTY_ARR,
    DA_ERR_INVALID_CAPACITY,
    DA_ERR_INVALID_OPERATION
}
DA_STATUS;

typedef enum da_resize_policy
{
    DA_RESIZE_INC,
    DA_RESIZE_DEC
} DA_RESIZE_POLICY;

static inline size_t da_new_capacity_inc(size_t old_capacity)
{
    return (old_capacity > 0) ? old_capacity * 2 : DA_MIN_CAPACITY;
}

static inline size_t da_new_capacity_dec(size_t old_capacity)
{
    return old_capacity / 2;
}

DynamicArray *da_create_dynamic_array(size_t elem_size, CompareFunc cmp_func);

DA_STATUS da_push_back(DynamicArray *da, void *elem);

DA_STATUS da_pop_back(DynamicArray *da);

DA_STATUS da_insert_at(DynamicArray *da, void *elem, size_t index);

DA_STATUS da_remove_at(DynamicArray *da, size_t index);

DA_STATUS da_remove_fst(DynamicArray *da, void *elem);

DA_STATUS da_remove_lst(DynamicArray *da, void *elem);

DA_STATUS da_remove_all(DynamicArray *da, void *elem);

void *da_get(DynamicArray *da, size_t index);

void *da_find(DynamicArray *da, void *elem);

DA_STATUS da_ensure_capacity(DynamicArray *da, DA_RESIZE_POLICY da_resize_policy);

DA_STATUS da_resize_inc(DynamicArray *da);

DA_STATUS da_resize_dec(DynamicArray *da);

DA_STATUS da_resize(DynamicArray *da, size_t new_capacity);

DA_STATUS da_free(DynamicArray *da);

DA_STATUS da_display(DynamicArray *da);

#endif