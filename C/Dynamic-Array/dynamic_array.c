#include <stdlib.h>
#include <string.h>
#include "dynamic_array.h"

DynamicArray *da_create_dynamic_array(size_t elem_size, CompareFunc cmp_func)
{
    if (elem_size < DA_MIN_ELEM_SIZE)
    {
        return NULL;
    }

    DynamicArray *da = malloc(sizeof(DynamicArray));
    if (da == NULL)
    {
        return NULL;
    }

    // Initial array attributes
    da->arr_size = 0;
    da->min_capacity = DA_MIN_CAPACITY;
    da->capacity = DA_MIN_CAPACITY;
    da->elem_size = elem_size;
    da->cmp_func = cmp_func;

    da->arr = malloc(da->capacity * da->elem_size);
    if (da->arr == NULL)
    {
        free(da);
        return NULL;
    }
    da->end = da->arr; // Initially start of array = end of array

    return da;
}

DA_STATUS da_push_back(DynamicArray *da, void *elem)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    DA_STATUS status = da_ensure_capacity(da, DA_RESIZE_INC);
    if (status != DA_OK)
    {
        return status;
    }

    memcpy(da->end, elem, da->elem_size);
    da->end += da->elem_size;
    da->arr_size++;

    return DA_OK;
}

DA_STATUS da_pop_back(DynamicArray *da)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    if (da->arr_size <= 0)
    {
        return DA_ERR_EMPTY_ARR;
    }

    da->end -= da->elem_size;
    da->arr_size--;

    DA_STATUS status = da_ensure_capacity(da, DA_RESIZE_DEC);
    if (status != DA_OK)
    {
        return status;
    }

    return DA_OK;
}

DA_STATUS da_insert_at(DynamicArray *da, void *elem, size_t index)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    if (index < 0 || index >= da->arr_size)
    {
        return DA_ERR_INDEX;
    }

    int status = da_ensure_capacity(da, DA_RESIZE_INC);
    if (status != DA_OK)
    {
        return status;
    }

    memmove(da_get(da, index + 1), da_get(da, index), (da->arr_size - index) * da->elem_size);
    memcpy(da_get(da, index), elem, da->elem_size);
    da->end += da->elem_size;
    da->arr_size++;

    return DA_OK;
}

DA_STATUS da_remove_at(DynamicArray *da, size_t index)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    if (index < 0 || index >= da->arr_size)
    {
        return DA_ERR_INDEX;
    }

    memmove(da_get(da, index), da_get(da, index + 1), (da->arr_size - index - 1) * da->elem_size);
    da->end -= da->elem_size;
    da->arr--;

    int status = da_ensure_capacity(da, DA_RESIZE_DEC);
    if (status != DA_OK)
    {
        return status;
    }

    return DA_OK;
}

DA_STATUS da_remove_fst(DynamicArray *da, void *elem)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    for (size_t i = 0; i < da->arr_size; i++)
    {
        void *curr_elem = da_get(da, i);
        if (curr_elem != NULL && da->cmp_func(curr_elem, elem) == 0)
        {
            return da_remove_at(da, i);
        }
    }

    return DA_OK;
}

DA_STATUS da_remove_lst(DynamicArray *da, void *elem)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    for (size_t i = da->arr_size - 1; i >= 0; i--)
    {
        void *curr_elem = da_get(da, i);
        if (curr_elem != NULL && da->cmp_func(curr_elem, elem) == 0)
        {
            return da_remove_at(da, i);
        }
    }

    return DA_OK;
}

DA_STATUS da_remove_all(DynamicArray *da, void *elem)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    size_t removed = 0;
    size_t ptr1 = 0;
    for (size_t ptr2 = 0; ptr2 < da->arr_size; ptr2++)
    {
        void *curr_elem = da_get(da, ptr2);
        if (da->cmp_func(curr_elem, elem) == 0)
        {
            removed++;
        }
        else
        {
            memcpy(da_get(da, ptr1), curr_elem, da->elem_size);
            ptr1++;
        }
    }

    da->end -= removed * da->elem_size;
    da->arr_size -= removed;

    int status = da_ensure_capacity(da, DA_RESIZE_DEC);
    if (status != DA_OK)
    {
        return status;
    }

    return DA_OK;
}

void *da_get(DynamicArray *da, size_t index)
{
    if (da == NULL)
    {
        return NULL;
    }

    if (index < 0 || index >= da->arr_size)
    {
        return NULL;
    }

    return da->arr + (index * da->elem_size);
}

void *da_find(DynamicArray *da, void *elem)
{
    if (da == NULL || elem == NULL)
    {
        return NULL;
    }

    for (void *start = da->arr; start != da->end; start += da->elem_size)
    {
        void *curr_elem = start;
        if (da->cmp_func(curr_elem, elem) == 0)
        {
            return curr_elem;
        }
    }

    return NULL;
}

DA_STATUS da_ensure_capacity(DynamicArray *da, DA_RESIZE_POLICY da_resize_policy)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    switch (da_resize_policy)
    {
        case DA_RESIZE_INC:
            if (da->arr_size >= da->capacity)
            {
                return da_resize_inc(da);
            }
            break;

        case DA_RESIZE_DEC:
            if (da->arr_size <= da->capacity / 2)
            {
                return da_resize_dec(da);
            }
            break;

        default:
            return DA_ERR_INVALID_OPERATION;
    }

    return DA_OK;
}

DA_STATUS da_resize_inc(DynamicArray *da)
{
    return da_resize(da, da_new_capacity_inc(da->capacity));
}

DA_STATUS da_resize_dec(DynamicArray *da)
{
    return da_resize(da, da_new_capacity_dec(da->capacity));
}

DA_STATUS da_resize(DynamicArray *da, size_t new_capacity)
{
    // Valid Dynamic Array
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    // New capacity must be eqaul to or greater than the current array size
    if (new_capacity < da->arr_size)
    {
        return DA_ERR_INVALID_CAPACITY;
    }

    void *temp = realloc(da->arr, new_capacity * da->elem_size);
    if (temp == NULL)
    {
        return DA_ERR_ALLOC;
    }

    da->arr = temp;
    da->end = da->arr + da->arr_size * da->elem_size;
    da->capacity = new_capacity;

    return DA_OK;
}

DA_STATUS da_free(DynamicArray *da)
{
    if (da == NULL)
    {
        return DA_ERR_NULL_ARR;
    }

    free(da->arr);
    free(da);

    return DA_OK;
}
