# Redirection

## Redirecting IO
You probably noticed that your programs have been recieving input from standard input (0), outputing to standard out(1), and errors in standard error(2). However, you can change where a program recieves its input or sends its output by modifying which files the file descriptors map or refer to.

### By Default
- `stdin` (`0`): Receives input from the terminal.
- `stdout` (`1`): Sends normal output to the terminal.
- `stderr` (`2`): Sends error messages to the terminal.

By Using redirection you can make these fiile descriptors refer to some other different file.

---

## 1. Redirection: Shell Commands
| Command | Description |
|---|---|
| `cmd < infile` | Redirect `stdin(0)` to read from `infile`. |
| `cmd > outfile` | Redirect `stdout(1)` to `outfile`, truncating existing content. |
| `cmd >> outfile` | Redirect `stdout(1)` to `outfile`, appending to existing contents. |
| `cmd 1> outfile` | Explicitly redirect `stdout(1)` to `outfile`, truncating existing content. |
| `cmd 1>> outfile` | Explicitly redirect `stdout(1)` to `outfile`, appending to existing content. |
| `cmd 2> outfile` | Explicitly redirect `stderr(2)` to `outfile`, truncating existing content. |
| `cmd 2>> outfile` | Explicitly redirect `stderr(2)` to `outfile`, appending to existing content. |
| `cmd &> outfile` | Redirect both `stdout(1)` and `stderr(2)` to `outfile` |

**Note:** The input files must exist otherwise it will error. You can't read from a nonexistent file.

### Examples

You should run these commands in order.

#### Example 1
```bash
echo Hello World
```

1. `echo "Hello World"` - basic echo command, you should see the output in the terminal which in this case is `stdout (1)`.

#### Example 2
```bash
echo "Hello World" > hello.txt
```

1. `echo "Hello World"` - basic echo command that outputs "Hello World". However, you should not see this in the terminal.
2. `>` - redirects `stdout(1)` to `hello.txt`. If `files.txt` does not exist then it is created. If it already exist then it is truncated.
3. Notice now that the content is inside of `hello.txt`.

#### Example 3
```bash
grep -i "hello" < hello.txt
```

1. `grep -i "Hello"` - find all lines in input with the matching pattern. You can read more about this in the man pages. Next, you should see the output in `stdout(1)` which is the terminal.
2. What if you want to store the result somewhere else?

#### Example 4
```bash
grep -i "hello" < hello.txt > matches.txt
```

1. `>` - this time we added another redirection, we redirect the `stdout(1)` to `matches.txt`. So now all the output will appear in `matches.txt`
2. What about errors? what if we want a dedicated file for that as well?

#### Example 5
```bash
grep -i "hello" nonexistent.txt > matches.txt 2> error.txt
```

1. `2>` - as you can see this time we provided a nonexistent input file which should error. We redirected `stderr(2)` to `error.txt` so now all the error messages will be written to that file.

**Note:** Shell errors and program errors are differnt. `<`, `>`, `2>` and others are processed by the shell first before the command can execute.

**Shell Error:** If the redirection fails then the shell will report the error when and the command will not be executed.

**Program Error:** If the shell successfully redirects and the program executes, any error from the program will write to stderr if you specified the redirection.

```bash
# Because redirection fails here due to non existent file the program `grep` will not run and therefore will not produce any program error that should be written to error.txt. The reason why the previous command (Example 5) worked is because we directly provided the file name as an argument to the program `grep`. Try running the command then check inside `error.txt`, nothing should be there.
grep -i "hello" < nonexistent.txt > matches.txt 2> error.txt
```
---

## 2. Redirection: File Descriptor Table, Open File Table, and Inode Table

Every single process has their own **File Descriptor Table**. An entry in the file descriptor maps to an entry in the **Open File Table**. That entry in the open file table also maps to another entry in the **Inode Table**

| Feature | File Descriptor Table (FDT) | Open File Table (OFT) | Inode Table |
|---|---|---|---|
| Scope | Per Process | System Wide | File System |
| Lifetime | Exists when the process exists. | Entry exists while the open file description is being referenced by at least one file descriptor. |An entry in the Inode represents a file system object so it can exist even after the process closes it. If a process creates a file successfully, that file should persist after the program ends basically. |
| Purpose | Tracks a process's open file descriptors. | Track individual files that are open and their current state. | Track any file in the filesystem and their metadata.|
| Contains | FD entries referencing open file decriptions. | File offset, status flags (read only, write only, ...), and inode reference. | Metadata of file (Permissions, size, location on disk) |
| Shared | Each process has their own table. | Entries can be shared by multiple FDs. | Multiple openings can share the same inode reference.|

### In Short
- **File Descriptor Table (FDT):** A per process table that maps file descriptors to entries in the Open File Table.
- **Open File Table (OFT):** A system wide table that tracks individual file openings, including their file offsets, access modes, and references entries in the Inode Table.
- **Inode Table:** A filesystem table that stores file metadata, like permissions, size, and the location of the file content.

```
[File Descriptor Table] -> [Open File Table] -> [Inode Table]
```

## 3. Redirection: dup() and dup2()
```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

| Function | Description | Parameter | Return |
|---|---|---|---|
| `dup` |  Assigns a new lowest available file descriptor to reference the same Open File Table entry as the original old file decriptor. | <ol><li>`oldfd` - The existing valid open file descriptor to duplicate.</li></ol>| <ul><li>**On Success** - Returns `>=0` which is the new lowest available file descriptor.<li><li>**On Failure** - Returns `-1` and global `errno` is set.</li></ul> |
| `dup2` | Assign the specified file descriptor to reference the same Open File Table entry as the original old file descriptor. If the specified file descriptor is already open, it automatically closes it first. | <ol><li>`oldfd` - The existing valid open file descriptor to duplicate.</li><li>`newfd` - File descriptor number to create or replace</li></ol> | <ul><li>**On Success** - Returns `>= 0` the specified file descriptor.</li><li>**On Failure** - Returns `-1` and global `errno` is set.</li></ul> |

### Example: dup()
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    // Open a file for reading and writing, create if not exist, with permission 0644.
    int fd1 = open("file.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd1 < 0) {
        // Always check if open() failed.
        perror("open");
        exit(EXIT_FAILURE);
    }

    /*
        This will assign a new lowest available file descriptor to reference
        the same Open File Table entry as the original old file descriptor.

        This means fd2 will point to the same Open File Table entry as fd1.
    */
    int fd2 = dup(fd1);
    if (fd2 < 0) {
        // Always check if dup() failed.
        perror("dup");
        exit(EXIT_FAILURE);
    }
    // Print the fd values
    fprintf(stdout, "fd1: %d\nfd2: %d\n", fd1, fd2);

    // !!!!! Write Message with fd1
    char write_message[] = "Hello World!";
    ssize_t bytes_written = write(fd1, write_message, sizeof(write_message));

    // Check the number of bytes written.
    if (bytes_written >= 0) {
        fprintf(stdout, "Wrote to fd1(%d): %.*s\n", fd1, (int)bytes_written, write_message);
    } else {
        // Check if write failed.
        perror("write");
    }

    // Go to beginning of file for reading.
    lseek(fd1, 0, SEEK_SET);

    // !!!!! Read Message with fd2
    char read_message[1024];
    ssize_t bytes_read = read(fd2, read_message, 1023);

    // Check the number of bytes read.
    if (bytes_read >= 0) {
        read_message[bytes_read] = '\0';
        fprintf(stdout, "Read from fd2(%d): %s\n", fd2, read_message);
    } else {
        // Check if read failed.
        perror("read");
    }

    // Please do not forget to close the open file descriptors when you are done.
    close(fd1);
    close(fd2);

    return 0;
}
```

**Note:** After running this program you will notice that `fd1` was used to write to the file and `fd2` was used to read from the file. Since `fd2` was created using `dup(f1)`, both of the file descriptors reference the same Open File Table entry and therefore are accessing the same exact file.

**Important Stuff:** Because they are accessing the same Open File Table entry after we used `dup()` they are also also sharing the same open file description (the file offset, status_flag, mode, and more). That was why we had to use `lseek()` to go to the beginning of the file to start reading.

### Example: dup2()
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    // Open a file for reading and writing, create if not exist, with permission 0644.
    int fd1 = open("file.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd1 < 0) {
        // Always check if open() failed.
        perror("open");
        exit(EXIT_FAILURE);
    }

    /*
        This will assign the specified file descriptor to reference the
        same Open File Table entry as the original old file descriptor.

        This means fd2 in this case `5` will point to the same Open File Table entry as fd1.
    */
    int fd2 = dup2(fd1, 5); // I chose 5 as the newfd. You can choose whatever you need.
    if (fd2 < 0) {
        // Always check if dup2() failed.
        perror("dup");
        exit(EXIT_FAILURE);
    }
    // The fd values
    fprintf(stdout, "fd1: %d\nfd2: %d\n", fd1, fd2);

    // !!!!! Write Message with fd1
    char write_message[] = "Hello World!";
    ssize_t bytes_written = write(fd1, write_message, sizeof(write_message));

    // Check the number of bytes written.
    if (bytes_written >= 0) {
        fprintf(stdout, "Wrote to fd1(%d): %.*s\n", fd1, (int)bytes_written, write_message);
    } else {
        // Check if write failed.
        perror("write");
    }

    // Go to beginning of file for reading.
    lseek(fd1, 0, SEEK_SET);

    // !!!!! Read Message with fd2
    char read_message[1024];
    ssize_t bytes_read = read(fd2, read_message, 1023);

    // Check the number of bytes read.
    if (bytes_read >= 0) {
        read_message[bytes_read] = '\0';
        fprintf(stdout, "Read from fd2(%d): %s\n", fd1, read_message);
    } else {
        // Check if read failed.
        perror("read");
    }

    // Please do not forget to close the open file descriptors when you are done.
    close(fd1);
    close(fd2);

    return 0;
}
```

**Note:** Same thing here, except we choose the file descriptor. Also, generally the `dup2()` function is more commonly used because it is not as limited in terms of the file descriptor values.

## 4. Redirection: fork() and exec()

The shell redirection commands we talked about earlier can also be implemented in your C programs using `fork()`, `exec()`, and `dup2()`.

### Example 1
Shell Command
```bash
echo "Hello World!\nGoodbye World!\nHello World Again!" > hello.txt
```

C Program
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child process
        /*
            Open the file inside the child process.

            !!!!! It not completely wrong to open it in the parent process. However, the child will
            inherit the file descriptor, so now both the parent and child has to manage their copies
            when they are no longer needed. If you open the file in the child, you can avoid unnecessary
            file descritor management in the parent.
        */
        int output_fd = open("hello.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            // Always check if open() failed.
            perror("open");
            _exit(EXIT_FAILURE);
        }

        /*
            By default the `echo` program will output to standard out (1).

            However, we want it to output to `hello.txt`. Since we already have a
            file descriptor (`output_fd`) that maps to a Open File Table entry for `hello.txt`
            after calling the open() function, what we can do is redirect standard out (1)
            to reference the same Open File Table entry as `output_fd`.
        */
        if (dup2(output_fd, STDOUT_FILENO) < 0) {
            // Always check if dup2() failed.
            perror("dup2");
            _exit(EXIT_FAILURE); // Terminate Child
        }

        /*
            Because we redirected standard out (1) to reference the same
            Open File Table entry as `output_fd`, we essentially have two file descriptors
            referencing the same Open File Table entry. It is best to close the one
            we don't need anymore which is the old `output_fd`.
        */
        close(output_fd);

        // Run the program with exec.
        execlp("echo", "echo", "Hello World!\nGoodbye World!\nHello World Again!", NULL);

        // Should not be here unless exec failed.
        perror("execlp");
        _exit(EXIT_FAILURE); // Terminate Child

    } else {
        // Parent Process
        fprintf(stdout, "Parent waiting for Child: %d\n", pid);
        int status;
        pid_t waited_child = waitpid(pid, &status, 0);
        fprintf(stdout, "Parent done waiting for Child: %d\n", pid);

        // Check if wait failed.
        if (waited_child == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Check if the waited child is the one the parent spawned.
        if (waited_child == pid) {
            if (WIFEXITED(status)) {
                fprintf(stdout, "Child Exit Code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stdout, "Child Killed by Signal: %d\n", WTERMSIG(status));
            }
        }
    }

    return 0;
}
```

### Example 2
```bash
grep -i "hello" < hello.txt > matches.txt
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child Process

        /*
            This is the file descriptor we will use to read from a file for input.
        */
        int input_fd = open("hello.txt", O_RDONLY);
        if (input_fd < 0) {
            // Always check if open() failed.
            perror("open input");
            _exit(EXIT_FAILURE);
        }

        // Redirect standard in (0) to the Open File Table entry that is referenced by`input_fd`.
        if (dup2(input_fd, STDIN_FILENO) < 0) {
            // Always check if dup2() failed.
            perror("dup2 input");
            _exit(EXIT_FAILURE);
        }
        // Since we redirected standard in (0) to the Open File Table Entry also referenced
        // by `input_fd`, we can close `input_fd` since we don't need it anymore.
        close(input_fd);

        /*
            This is the file descriptor we will use to write to a file for output.
        */
        int output_fd = open("matches.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            // Always check if open failed.
            perror("open output");
            _exit(EXIT_FAILURE);
        }

        // Redirect standard out (1) to the Open File Table entry that is referenced by `output_fd`.
        if (dup2(output_fd, STDOUT_FILENO) < 0) {
            // Always check if dup2() failed.
            perror("dup2 output");
            _exit(EXIT_FAILURE);
        }
        close(output_fd);

        // Run the program with exec.
        execlp("grep", "grep", "-i", "hello", NULL);

        // Should not be here unless exec failed.
        perror("execlp");
        _exit(EXIT_FAILURE); // Terminate Child

    } else {
        // Parent Process
        fprintf(stdout, "Parent waiting for Child: %d\n", pid);
        int status;
        pid_t waited_child = waitpid(pid, &status, 0);
        fprintf(stdout, "Parent done waiting for Child: %d\n", pid);

        // Check if wait failed.
        if (waited_child == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Check if the waited child is the one the parent spawned.
        if (waited_child == pid) {
            if (WIFEXITED(status)) {
                fprintf(stdout, "Child Exit Code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stdout, "Child Killed by Signal: %d\n", WTERMSIG(status));
            }
        }
    }

    return 0;
}
```

**Note:** Please remember to close any unused or unnecessary file descriptors because they are inherited by the child process or new program. The purpose is to free system resources and prevent unexpected behavior, especially important for pipes.

## 5. Important Stuff
1. If you want to run a new program, but you want to redirect the standard out (1) or other standard file descriptors to a another file, then you should open the file in the the child process before exec to avoid unnecessary file descriptor management in the parent.
2. Redirect in child process before calling exec. If you do it after calling exec the code will never execute if it succeeds.
3. Don't ever forget to close unnecessary file descriptors, otherwise this might cause resource leaks or unexpected behavior.
4. `dup()` and `dup2()` does not copy file content it just redirect where the file descriptor references on the Open File Table.
5. All file descriptors are automatically closed by the operating system after the program terminates.