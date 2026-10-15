# Processes

---

## 1. Process Creation - fork()

To create or spawn a child process from a parent process, we need to use the `fork()` function.

```c
#include <unistd.h>

pid_t fork(void);
```

Purpose: Creates a child process by **duplicating** or **copying** the parent processes.

| Return | Meaning |
|---|---|
| -1 | Failed to create child. |
| 0 | This is the child process. |
| > 0 | This is the parent process. |

Important Stuff
- Both processes continue executing right after **fork()**. The child does not restart main.
- The execution order depends on the OS scheduler, so you won't know which of parent or child will execute first.
- Each process has their own Process ID (PID).

### Example: Basic Parent and Child Process With Fork
```c
// process_fork.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // Needed for fork()
#include <sys/types.h> // Needed for pid_t type
#include <sys/wait.h>  // Needed for wait()

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Always make sure fork() worked.
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        // Child does some work ...
        printf("Hello from Child.\n");
        _exit(0); // Destroy the child process.
    } else {
        // Parent Process
        // Parent does some work ...
        printf("Hello from Parent.\n");
        wait(NULL); // Reap the child resources to prevent zombie process.
    }

    return 0;
}
```

**Critical:** Without calling `_exit()` in the child branch will cause the child branch to also execute code that is intended for the parent when it leaves its branch.

---

## 2. What Does fork() Copy?

| Component | Is it Copied from Parent to Child? |
|---|---|
| Program Code | No, the parent and child process will share the code segment / text segment because the code is read only. |
| Stack / local variables |  Yes, they will have their own independent copies. |
| Heap / dynamic allocated memory | Yes, they will have their own independent copies. |
| Global / Static Variables | Yes, they will have their own independent copies. |
| Execution State | Both will resume after the fork statement. |
| Output Buffers | Yes, unflushed user space buffer data will be copied. |
| File descriptors | Yes, the child process will receive a copy of the parent process's file descriptor table. However, note that the underlying file object that the file descriptors point to are still the same as the parent's. |
| Process ID (PID) | No, the child process will receive a new process ID. |

Note: the data is not immediately copied and instead follows Copy-on-Write (COW).
1. Initially the Parent and the Child process will share the same page in Memory.
2. When either makes a change to a shared page, the operating system will create a private copy on a seperate page in Memory.
3. Changes to a normal variable to one process will not affect the other.

### Example: Independent Memory for Parent and Child Process.
```c
// process_independent_mem.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int x = 10;
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        x += 10; // Modifies x value, causes copy on write.
        printf("Child Process - x value is: %d\n", x); // Should be 20.
        _exit(0); // Destroy the child process.
    } else {
        // Parent Process
        x += 20;
        printf("Parent Process - x value is: %d\n", x); // Should be 30.
        wait(NULL); // Reap the child resources to prevent zombie process.
    }
    return 0;
}
```

---

## 3. Output Buffering and fork()

Standard IO functions like `printf()` can sometimes store the output in the user space output buffer. The user space gets emptied when calling `fflush()` before it is written to the output buffer in the kernel space and eventually written to the physical disk.

**Note:** Unflushed buffers are copied when `fork()` is called.
```c
// process_output_buf.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/type.h>
#include <sys/wait.h>

int main() {
    printf("Hello"); // There is a chance this gets printed twice - "HelloHello".
    pid_t pid = fork();
    // Check fork ... and wait ...
    return 0;
}
```

Flush before forking
```c
// process_output_buf.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/type.h>
#include <sys/wait.h>

int main() {
    printf("Hello");
    fflush(stdout);  // Flush before forking.
    pid_t pid = fork();
    // Check fork ... and wait ...
    return 0;
}
```

---

## 4. Process Identification

```c
#include <unistd.h>

pid_t getpid(void);  // Returns the current process PID.
pid_t getppid(void); // Returns the current process's parent PID.
```

### Example: Parent and Child PID
```c
// process_id.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        pid_t my_pid = getpid();
        pid_t parent_pid = getppid();
        printf("Child Process - My PID is %d - My Parent's PID is %d\n", my_pid, parent_pid);
        _exit(0); // Destroy the child process.
    } else {
        // Parent Process
        pid_t my_pid = getpid();
        printf("Parent Process - My PID is %d - My Child's PID is %d\n", my_pid, pid);
        wait(NULL); // Reap the child resources to prevent zombie process.
    }
    return 0;
}
```

## 5. Waiting For Children - wait()
```c
#include <sys/wait.h>

pid_t wait(int *status);
```

**Purpose:** Wait for any child to terminate and reap it (Remove it from the process table).

| Parameter | Meaning |
|---|---|
| int *status | A pointer to int variable that will store the child's termination information. Pass in NULL is you want to ignore the termination info. |

| Return | Meaning |
|---|---|
| > 0 | PID of reaped child. |
| -1 | Some error happened. |

**Note:** The parent **blocks** in wait() until any of its child terminates.

### Example: Using wait()
```c
// process_wait.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        // Child does some work ...
        _exit(0); // Destroy the child process.
    } else {
        // Parent Process
        // Parent does some work ...
        wait(NULL); // Reap the child resources to prevent zombie process.
    }
    return 0;
}
```

---

## 6. Waiting for Specific Children - waitpid()
```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
```

**Purpose:** Wait for selected child processe with additional control options.

### pid Values
| Value | Behavior |
|---|---|
| > 0 | wait specific child with this pid. |
| -1 | wait for any child. |

### Important Options
| Option | Meaning |
|---|---|
| 0 | Block until specified child terminates. |
| WNOHANG | Do not block, check immediately **if** a child has terminated and return.|
| WUNTRACED | Return if a child has terminated **OR** if a child was paused/stopped. |
| WCONTINUED | Return if a child has terminated **OR** if a stopped child was resumed. |

You can also combine options via the bitwise OR operator `|`.
```c
waitpid(pid, &status, WNOHAND | WUNTRACED);
```

### Return Value
| Return | Meaning |
|---|---|
| > 0 | PID of the child that was checked on. |
| 0 | WNOHANG option was used and no change was found with the child process's state. |
| -1 | Some error happened. |

**Note:** The wait functions does not reap the child unless it returns after catcing that the child was terminated. If WNOHANG options was used and the wait function returns after catching that the child is still running, then the child is not reaped and need to be checked up again later. Same thing with WUNTRACED and WCONTINUED when they return after catching child stopping or resuming respectively.

---

## 7. Interpreting Child Exit Status

You can interpret the status that the wait functions return with existing macros.
Make sure to include `<sys/wait.h>` header to use the following macros.
| Macro | Purpose |
|---|---|
| WIFEXITED(status) | Child terminated normally. |
| WEXITSTATUS(status) | Obtain normal exit code. |
| WIFSIGNALED(status) | Child was terminated by a signal. |
| WTERMSIG(status) | Obtain terminating signal. |
| WIFSTOPPED(status) | Child was stoppe. |
| WSTOPSIG(status) | Obtain stopping signal. |
| WIFCONTINUED(status) | Check whether the child resumed after previously stopped. |

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child Process
        // Child does some work ...
        _exit(0); // Destroy the child process.
    } else {
        // Parent Process
        // Parent does some work ...
        int status;
        pid_t result = waitpid(pid, &status, 0); // Reap the child resources to prevent zombie process.
        if (result == pid) {
            if (WIFEXITED(status)) {
                // Normal exit
                printf("Exit Code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                // Terminated by a certain signal.
                printf("Killed by signal: %d\n", WTERMSIG(status));
            }
        }
    }
    return 0;
}
```

---

## 8. Interpreting Error Codes From wait() and waitpid()

If an error occurs with the wait functions, the global `errno` will be set.

### ECHILD
- `wait()` - For `wait()` This error occurs when the calling process has no child processes left to wait for. (Either they are all reaped or never existed in the first place).

- `waitpid()` - For `waitpid()` This error occurs when the specifc child pid you specified does not exist, or it is not the child of the calling process. If you use -1 as the pid, then it acts like `wait()`.

### EINTR
- This error happens when the wait funtions were interrupted by a signal before it is able to capture the state of the child process.

### EINVAL
- This error applies to `waitpid()` and it happens when you pass an invalid option flag to `waitpid()`

---

## 9. Zombie and Orphan Processes

## Zombie processes
A **zombie process** is a child process that terminated but the exit status was not read or collected by its parent. Even though most of the physical memory resources are cleaned up after the child terminates, an entry of this terminated process still exist in the process table with its status code. Since the number of PIDs is finite you will eventually run out of PIDs and fail to create a new one. This is why it is extremely important to use `wait()` or `waitpid()` to reap the child processes.

## Orphaned processes
An **orphaned process** is a child process that kept running but its parent process finished or terminated without waiting for them. The the child process goes gets adopted by another special process like `init` that will handle the waiting and reaping.