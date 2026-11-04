# Threads

Creating a new process can be expensive because each process needs it own virtual address space. However, what if we eant something more light weight? That is where **threads** come in.Instead of having seperate virtual address spaces, threads within the same process will share **one single virtual address space**. This makes creating threads faster and less expensive than creating new processes.

## Process and Threads Comparison

| Feature | Processes | Threads |
|---|---|---|
| Virtual Address Space | Seperate | Shared |
| Global Variables | Seperate Copies after fork() | Shared |
| Heap Memory | Seperate | Shared |
| Stack | Each process has it own stack. | Each thread has their own stack. |
| Open file descriptors | Seperate file descriptor tables, but can share open file entries. | Shared file descriptor table. |
| Communication | Inter process communication, like using pipes. | Shared memory. |

### Why use Threads?

1. Light weight: Creating a new thread requires less resources.
2. Easier Communication: Directly access shared memory.
3. Better CPU Utilization: When one thread blocks, another thread can be doing something else.
4. Concurrency: Multiple threads can work on different tasks at the same time.
5. Parallelism: On multi core CPU, threads can be run simultaneously which can improve performance.

### Issues with Threads
Since threads of the same process share a single virtual address space, one thread can modify data another thread is usually caused by race conditions. This would not have happened with process because each process have their own virtual address space, so modifying one variable in one process does not affect the other.

## Concurrency and Parallelism

At any point in time, a single CPU core can ONLY run one thread of one process at a time.

### Concurrency - Single Core CPU
**How it works:** A single core runs only one instruction or thread at a time.

**Illusion of multitasking:** The operating system themselves use a **scheduler** to switch back and forth between programs, processes, or threads (This is called context switching). Also, this happens so fast that it create the illusion of parallelism where many tasks are done at the same time, but it actually just quickly switching bettwen tasks.

### Parallelism - Multi Core CPU
**How it work:** Each core in the CPU can run a seperate program, process, or thread at a single point in time. This is true parallelism, though you do use more power.

## Pthread API
Below are some functions you use when writing programs with threads, but there are also others.

### Creating a Thread
```c
#include <pthread.h>

int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg
);
```
| Function | Description | Parameter | Return |
|---|---|---|---|
| `pthread_create` | Creates a new thread. | <ol><li>`thread` - Stores the id of the newly created thread.</li><li>`attr` - Thread attributes, use `NULL` for defaults.</li><li>`start_routine` - Function the new thread executes.</li><li>`arg` - Arguments passed to the thread function.</li></ol> | <ul><li>**On Success:** Returns `0`, thread created successfully.</li><li>**On Failure:** Return nonzero</li></ul>|

#### Understanding the Main Thread Function
Every thread starts by executing a function with this signature.
```c
void *thread_main(void *arg);
```
- `void *` - The type of the parameter is `void *` so that it allows us to pass in any type of arguments.
- `void *` - The return type of the function allows the function to return any type as the result.
- `thread_main` - Just the name of the function can be anything else you want to name it.

**Note:** This is basically the function you pass in as the `start_routine` argument for `pthread_create`.

### Joining A Thread
```c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
```

| Function | Description | Parameter | Return |
|---|---|---|---|
| `pthread_join` | Allows one thread to wait for another thread to finish execution, and collect its resources. | <ol><li>`thread` - ID of the thread to wait for.</li><li>`retval` - Stores the thread's return value. Use `NULL` if you don't need it.</li></ol>| <ul><li>**On Success:** Returns `0`, successfully joined the thread.</li><li>**On Failure:** Returns nonzero, some error occured.</li></ul> |

**Note:** `pthread_join` is a blocking call, it will wait for the specified thread to terminate.

### What Resources are Reclaimed?
Any resources left that can be reclaimed will be reclaimed after calling `pthread_join`.
- Thread Stack
- Thread Control Information
- Thread Termination Result

### What Resources are not Reclaimed?
Since some of the resources listed below can not be reclaimed you must manually clean them up when the thread using them no longer need them.
- Global Variables
- Heap Allocations
- Process Virtual Address Space.
- File dscriptors shared

### What if created Threads are not Joined?
| Situation | What Happens? |
|---|---|
| The main thread returns before the worker finishes. | If you return from the `main()` function, the process running the program is terminated and therefore all the threads of the process is also terminated. |
| The worker threads finishes, but the main thread continues to run without joining the terminated worker threads. | Since the program continues to run after the worker threads terminate, some resources remain allocated but unused, eventually you will run out of resources to create new threads. |

## Detaching a Thread
| Function | Description | Parameter | Return |
|---|---|---|---|
| `pthread_detach` | Make an existing joinable thread as detached so that it remaining resources are reclaimed when it terminates. | <ol><li>`thread` - The thread id of the existing joinable thread to detach</li></ol>| <ul><li>**On Success:** Returns `0`</li><li>**On Failure:** Return nonzero.</li></ul>|

**Note:** `pthread_detach` can also detach the calling thread.
## Terminating a Thread
```c
#include <pthread.h>

void pthread_exit(void *retval);
void pthread_cancel(pthread_t thread);
```
| Function | Description | Parameter | Return |
|---|---|---|---|
| `pthread_exit` | Terminate the calling thread without terminating the entire process. | <ol><li>`retval` - Pointer to the thread's termination result. This result can be collected by another thread that called `pthread_join` on this thread. Use `NULL` if the result is not needed.</li></ol> | None |
| `pthread_cancel` | Instead of terminating the calling thread, **Send a request to terminate another thread**. | <ol><li>`thread` - ID of the thread you want to request for cancelation.</li></ol> | <ul><li>**On Success:** Returns `0`, cancellation request successfully sent.</li><li>**On Failure:** Returns nonzero.</li>></ul>|

**Note:**
1. This is different from `exit()` which terminates the entire process and its threads. 2. Cancellation is a **request**, it does not mean the thread specified will terminated immediately. The thread to cancel/terminate must reach a cancellation point(`read`, `write`, `sleep`, ...)

## Thread Identity
```c
#include <pthread.h>

pthread_t pthread_self(void);
int pthread_equal(pthread_t t1, pthread_t t2);
```
| Function | Description | Parameter | Return |
|---|---|---|---|
| `pthread_self` | Returns the ID of the calling thread. | None | ID of calling thread. |
| `pthread_equal` | Check whether two thread IDs refer to the same thread. | <ol><li>`t1` - Thread ID 1</li><li>`t2` - Thread ID 2</li></ol>| <ul><li>**True:** Returns nonzero</li><li>**False:** Returns `0`</li></ul>|


## Race Condition

Race condition happens when the behovior or the result of a program depend on the timing or order multiple threads are executed. This is a very big problem especially when multiple threads access shared data without synchronization.

### Example - Race Condition
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 2
#define MAX_INCREMENT 100000

// Define the structure for thread arguments.
typedef struct thread_data {
    int thread_num;
    int *sum;
} TData;

void *thread_main(void *targs) {
    // Cast the argument to the correct type.
    TData *thread_data = (TData *)targs;

    // Collect the thread arguments
    int thread_num = thread_data->thread_num;
    int *sum = thread_data->sum;

    /*
        Increment by 1 to the shared variable "sum".

        However, sometimes you will notice that the result is not what you expected.
    */
    for (int i = 0; i < MAX_INCREMENT; i++) {
        (*sum)++;
    }

    // Terminate the thread.
    pthread_exit(NULL);
}

int main() {
    // Declare the array of threads ids and threads datas
    pthread_t thread_ids[MAX_THREADS];
    TData thread_datas[MAX_THREADS];

    int sum = 0;
    // Initialize the thread arguments
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_datas[i].thread_num = i;
        thread_datas[i].sum = &sum;
    }

    int status;
    // Create the threads.
    for (int i = 0; i < MAX_THREADS; i++) {
        status = pthread_create(
            &thread_ids[i],          // Pointer to Thread ID
            NULL,                    // Pointer to Attribute
            thread_main,             // Pointer to worker thread main function
            (void *)&thread_datas[i] // Pointer to worker thread data and arguments
        );

        // Always check if pthread_create failed.
        if (status != 0) {
            fprintf(stderr, "pthread_create failed with thread number: %d", i);
            exit(EXIT_FAILURE);
        }
    }

    // Join the threads.
    for (int i = 0; i < MAX_THREADS; i++) {
        status = pthread_join(thread_ids[i], NULL);

        // Always check if pthread_join failed.
        if (status != 0) {
            fprintf(stderr, "pthread_join failed with thread number: %d", i);
            exit(EXIT_FAILURE);
        }
    }

    /*
        Print the final sum.

        In this case because there are 2 threads we should be expecting
        the result to be 200000, however that is not always the case. Since the
        threads are accessing the shared variable "sum" without any protection,
        race conditions will occur.

        What exactly is happening though?

        The Operating System can just run 1 thread forever so it context switch between different threads.
        Note that there is no specific order in which threads get run next, it depends on the scheduler.

        Imagine this:
        1. Thread 1 reads variable "sum" - It saves that value (lets say 0) in its register.
        2. Context switch happens - Passes the turn to Thread 2.
        3. Thread 2 reads variable "sum" - It saves that value (which is still 0) in its register.
        4. Thread 2 reads next instruction to increment - Increment the value saved in register by 1.
        5. Thread 2 writes the result to memory - the value of the variable "sum" should now be 1.
        6. Repeat step 3 - 5 (50 times) - The value of the variable "sum" should now be 51.
        7. Context switch happens - Passes the turn to Thread 1.
        8. Thread 1 reads next instruction to increment - Increment the value saved in register (which is 0) by 1.
        9. Thread 1 writes the result to memory - The value of the variable "sum" should now be 1

        Note: As long as threads are accessing the same memory location (the address) then race condition will happen, unless you add protection.
    */
    fprintf(stdout, "Final Sum - Expected: %d - Got: %d\n", MAX_THREADS * MAX_INCREMENT, sum);
    return 0;
}
```

## Embarrassingly Parallel Problems

Some problems can be very easily split into smaller workloads to be distributed between multiple threads without much coordination or any shared part of the memory between the threads.

ex. Matrix Problems, 3D Rendering, and others.

### Common Equation to Use
There is a common way you can use to distribute the problems to each of the worker threads.
```
base_size = num_work / num_threads
remainder = num_work % num_threads

offset = thread_num * base_size + (thread_num < remainder ? thread_num : remainder)
count = base_size + (thread_num < remainder ? 1 : 0)
```

1. `base_size` - The base size of work for each thread.
2. `remainder` - The remaining work that can't be evenly distributed.
3. `off_set` - The starting index of the work.
4. `count` - The actual amount of work.

We will distribute these to first `remainder` threads. Basically, if there are 10 threads, and `remainder` is 5, we will distribute the 1 work to each of the first 5 threads, so now threads 0 - 4 has `base_size + 1` work and threads 5 - 9 has `base_size` work.

**Note:** This will be a bit confusing at first.

### Example: Sum Whole Array
Lets say we have the following of an `int` array:
1. `array_elements` = 14
2. `num_threads` = 4

Now we calculate this:
1. `base_size` = `array_elements` / `num_threads` = 3
2. `remainder` = `array_elements` % `num_threads` = 2 (The first two threads get 1 extra)

| Thread Number (`thread_num`) | Number of Work (`count`) | Calcuation for `offset` | `start_index` | `end_index` (exclusive) |
|---|---|---|---|---|
| 0 | 3 + 1 = 4 | 0 * 3 + 0 = 0 | 0 | 4 |
| 1 | 3 + 1 = 4 | 1 * 3 + 1 = 4 | 4 | 8 |
| 2 | 3 + 0 = 3 | 2 * 3 + 2 = 8 | 8 | 11 |
| 3 | 3 + 0 = 3 | 3 * 3 + 2 = 11 | 11 | 14 |

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 5
#define MAX_ARRAY_ELEMENTS 10003

typedef struct thread_data {
    int *arr;
    int thread_num;
    int start_index;
    int end_index;
    int result;
} TData;

void *sum_array(void *targs) {
    // Convert to correct type
    TData *thread_data = (TData *)targs;

    // Grab the data you need from thread_data.
    int *arr = thread_data->arr;
    int start_index = thread_data->start_index;
    int end_index = thread_data->end_index;
    int result = thread_data->result;

    for (int i = start_index; i < end_index; i++) {
        result += arr[i];
    }

    // Save the result
    thread_data->result = result;

    // Terminate the thread
    pthread_exit(NULL);
}

int main() {
    // The array containing the integers we want to sum
    int arr[MAX_ARRAY_ELEMENTS];
    for (int i = 0; i < MAX_ARRAY_ELEMENTS; i++) {
        arr[i] = i;
    }

    // !!!!! Declare the thread ids and thread data
    pthread_t thread_ids[MAX_THREADS];
    TData thread_datas[MAX_THREADS];

    // !!!!! Initalize the thread datas
    int base_size = MAX_ARRAY_ELEMENTS / MAX_THREADS;
    int remainder = MAX_ARRAY_ELEMENTS % MAX_THREADS;
    for (int i = 0; i < MAX_THREADS; i++) {
        // Do the calculations to split the work.
        int thread_num = i;
        int offset = thread_num * base_size + (thread_num < remainder ? thread_num : remainder);
        int count = base_size + (thread_num < remainder ? 1 : 0);

        // Now we have the start and end index.
        int start_index = offset;
        int end_index = offset + count;

        thread_datas[i].thread_num = thread_num;
        thread_datas[i].arr = arr;
        thread_datas[i].start_index = start_index;
        thread_datas[i].end_index = end_index;
        thread_datas[i].result = 0;
    }

    // !!!!! Create the threads
    int status;
    for (int i = 0; i < MAX_THREADS; i++) {
        status = pthread_create(
            &thread_ids[i],
            NULL,
            sum_array,
            (void *)&thread_datas[i]
        );

        if (status != 0) {
            // Always check if pthread_create failed.
            fprintf(stderr, "pthread_create failed with thread number: %d", i);
            exit(EXIT_FAILURE);
        }
    }

    // !!!!! Join the threads
    for (int i = 0; i < MAX_THREADS; i++) {
        status = pthread_join(thread_ids[i], NULL);
        if (status != 0) {
            // Always check if pthread_join failed.
            fprintf(stderr, "pthread_join failed with thread number: %d", i);
            exit(EXIT_FAILURE);
        }
    }

    // !!!!! Sum the result from each thread
    int sum;
    for (int i = 0; i < MAX_THREADS; i++) {
        sum += thread_datas[i].result;
    }

    fprintf(stdout, "Final Sum - Expected: %d - Got: %d\n",  ((1 + 10002) * 10002) / 2, sum);

    return 0;
}
```

## Important Stuff
1. Creating more threads does not always speed up computation. Creating new threads are not free and they also still need to be managed which adds complexity. Only create the number of threads necessary to finish a job.
2. Threads accessing the same part of the memory will experience race condition unless there are synchronization protection being placed.
3. Some problems are embarrassingly parallel, meaning they can be easily split into multiple smaller task each thread can take without accessing a shared part of the memory.