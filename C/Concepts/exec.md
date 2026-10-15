# Exec Family Functions

## 1. Running a Program in Forked Process

Sometimes you want your child processes to go and run a seperate program. This is possible with the exec family functions. When you call one of the exec functions it replaces the calling process image, preserves its PID and other things like file descriptors before running the new program. Essentially, the memory layout is replaced with a new one (code segment, stack, heap, global/static is replaced with new ones from the new program).

**Note:** Calling an exec function successfully does not create a new process.

## 2. Exec Functions

```c
#include <unistd.h> // For the exec functions.

int execl(const char *path, const char *arg0, ... /* NULL */);
int execlp(const char *file, const char *arg0, ... /* NULL */);

int execv(const char *path, char * const argv[]);
int execvp(const char *file, char * const argv[]);
```

| Exec Function Name Suffix | Description |
|---|---|
| `l` (List) | Arguments are passed as a comma seperated list of individual strings. You must terminate it with a `NULL` pointer. |
| `v` (Vector) | Arguments are passed as an array of string pointers. You must terminate it with a `NULL` pointer. |
| `p` (Path) | The function uses the `PATH` environment variable to search for the executable file. You only need to provide the filename instead of the full absolute path. If you type `echo $PATH` it will display all the file paths that it will search through for the executable. |

**Note:** If you included a `/` in the executable name for `execlp` or `execvp` then the funciton will not check the directories in `$PATH` and will instead treat it as a direct absolute or relative file path to the executable.

**Clarification:**

### Example: Calling execl()
```c
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child process

        // 1st parameter: we pass in the absolute filepath to the executable.
        // 2nd parameter - end: arguments of strings with NULL terminating.
        execl("/bin/echo", "/bin/echo", "Hello World", NULL);
        // The executable will be ran and the arguments after will be passed in as argv for the program.
        // So in this case, argv[0] = "echo", argv[1] = "Hello World".

        // Should not execute if exec succeeded otherwise exec failed. Terminate the child.
        perror("exec()");
        _exit(1);
    } else {
        // Parent process

        // Please do not forget to wait for the child.
        wait(NULL);
    }
    return 0;
}
```

### Example: Calling execlp()
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child process

        // 1st parameter: we pass the filename and the function will search in $PATH for the executable for us.
        // 2nd parameter - end: arguments of strings with NULL terminating.
        execlp("echo", "echo", "Hello World", NULL);

        // Should not execute if exec succeeded otherwise exec failed. Terminate the child.
        perror("exec()");
        _exit(1);
    } else {
        // Parent process

        // Please do not forget to wait for the child.
        wait(NULL);
    }
    return 0;
}
```

### Example: Calling execv()
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child process

        // Create the array of arguments first. It must end with NULL.
        char *args[] = {"/bin/echo", "Hello World", NULL};
        // 1st parameter: we pass in the absolute filepath to the executable.
        // 2nd parameter - end: an array of strings as arguments with NULL terminating.
        execv("/bin/echo", args);

        // Should not execute if exec succeeded otherwise exec failed. Terminate the child.
        perror("exec()");
        _exit(1);
    } else {
        // Parent process

        // Please do not forget to wait for the child.
        wait(NULL);
    }
    return 0;
}
```

### Example: Calling execvp()
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork()");
        exit(1);
    } else if (pid == 0) {
        // Child process

        // Create the array of arguments first. It must end with NULL.
        char *args[] = {"echo", "Hello World", NULL};
        // 1st parameter: we pass the filename and the function will search in $PATH for the executable for us.
        // 2nd parameter - end: an array of strings as arguments with NULL terminating.
        execvp("echo", args);

        // Should not execute if exec succeeded otherwise exec failed. Terminate the child.
        perror("exec()");
        _exit(1);
    } else {
        // Parent process

        // Please do not forget to wait for the child.
        wait(NULL);
    }
    return 0;
}
```

## 3. Important Stuff
- Exec does not create a new process, it transforms the existing calling process.

- Exec can fail so you need to handle cases where it does. Terminate the child that called on exec.

- If you included a `/` in the executable name for `execlp` or `execvp` then the funciton will not check the directories in `$PATH` and will instead treat it as a direct absolute or relative file path to the executable.

- Do not ever forget to put `NULL` termination in both argument list and argument array,

- PIDs are preserved so the process running the new program is actually just the same child process that called exec.

- File descriptors are also preserved, so any open file descriptors in the parent after fork is also accessible in the child and therefore the new running program.