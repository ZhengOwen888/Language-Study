# Inter Process Communication

Sometimes the parent spawns a child to run a seperate program, the child runs the program and exits. However, what if the parent wants the output of the program? Or what if the parent wants to communicate with the child to give it input? Or maybe child A wants to communicate with child B? If you recall, file descriptors are inherited by the child and the running program from the parent. We can use this feature to achieve our goal.

**Note:** There are many ways for inter process communication but this mostly focus on the use of pipes. Others include: files, named pipes, sockets, message queues, and shared memory

| The Method | Description |
|---|---|
| Files | Processes can exchange data by reading from and writing to files. |
| Pipes | Process can exchange data with a kernel managed circular buffer. |
| Named Pipes | Allow unrelated processes to communicate. |
| Sockets | Communicate over the network. |

**Note:** We focus on pipes.

## 1. Inter Process Communication: What is a Pipe?

A pipe is a unidirectional (one way) byte stream which allows one process to write bytes on one end to another process which reads from the other end using file descriptors. The Operating System kernel automatically manages the pipe's circular buffer for data transfer.

```
+-----------+             +----------------------+            +-----------+
[ Process A ]  Write (1)  [         Pipe         ]  Read (0)  [ Process B ]
[  Writer   ] --------->  [ Circular Byte Buffer ] ---------> [  Reader   ]
+-----------+             +----------------------+            +-----------+
```

### Example Shell Command with `|` (Pipe) Operator

The `|` operator connects the `standard out (1)` of one command to the `standard in (0)` of the next command.

Example 1
```bash
ls | wc
```

Example 2
```bash
echo 'Hello World!' | wc
```

Example 3
```bash
echo 'Banana\napple\nbanana\ncitrus\nApple' | tr A-Z a-z | sort | uniq
```

## 2. Inter Process Communication: pipe()
```c
#include <unistd.h>

int pipe(int pipefd[2]);
```

| Function | Description | Parameter | Return |
|---|---|---|---|
| `pipe`| Creates a unidirectional communication channel that allows processes to exchange data through a byte stream that is managed by the kernel. | <ol><li>`pipefd` - Array that recieves two file descriptors representing the read and write ends of the pipe.</li></ol>| <ul><li>**On Success** - Return `0`, both file descriptors are stored in `pipefd`.<li><li>**On Failure** - Return `-1` and global `errno` is set.</il></ul> |

| File Descriptor | Purpose | Operation |
|---|---|---|
| `pipefd[0]` | Read end of the pipe. | `read()` |
| `pipefd[1]` | Write end of the pipe. | `write()` |

```
+-----------+      +----------------------+      +-----------+
[ pipefd[1] ]      [         Pipe         ]      [ pipefd[0] ]
[ Write End ] ---> [ Circular Byte Buffer ] ---> [  Read End ]
+-----------+      +----------------------+      +-----------+
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    printf("Read FD: %d\n", pipefd[0]);
    printf("Write FD: %d\n", pipefd[1]);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

## 3. Extremely Important Stuff when Piping

Before writing programs with `pipe()`, there are some important things you need to take note of.

### Why is Closing Unused Pipe Endpoints Important?
!!! The operating system keeps track of whether any read or write endpoints (the read and write end of a pipe) remain open. It is extremely important to close any unused pipes in any process, as this is how `read()` and `write()` check if the communication is still possible. If you forget to close unused endpoints, it can prevent `EOF` detection and cause processes to block.

1. **Reading:** When a pipe is empty (nothing in buffer), `read()` will block as long as at least one write endpoint remain open.
2. **Writing:** When all the read endpoints are closed, writing to the pipe will cause `SIGPIPE` or `EPIPE` error.

| Pipe Condition | Behavior |
|---|---|
| Empty pipe, write end still open somewhere. | `read()` blocks |
| Empty pipe, all write ends closed. | `read()` returns `0` (EOF) |
| Pipe contains data. | `read()` reads available bytes up to specified amount.  |
| Pipe full, read end still open somewhere. | `write()` blocks. Wait until there is room in buffer. |
| All read ends closed. | `write()` will trigger `SIGPIPE` or `EPIPE`|

## 4. Parent to Child - Unidirectional (One Way Communication)
In this case the parent will write and the child will read.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        // Always check if pipe() failed.
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child Process

        // Close pipefd[1] because the child will not be writing in this case.
        close(pipefd[1]);

        char character;
        char read_buffer[1024];
        ssize_t bytes_read;
        unsigned int count = 0;
        while ((bytes_read = read(pipefd[0], &character, 1)) > 0) {
            if (character == '\n') {
                read_buffer[count] = '\0';
                count = 0;
                fprintf(stdout, "Child Recieved: %s\n", read_buffer);

            } else {
                if (count >= sizeof(read_buffer) - 1) {
                    fprintf(stderr, "Child: Message too long!\n");
                    _exit(EXIT_FAILURE);
                }
                read_buffer[count++] = character;
            }
        }

        // Check if we reached EOF, this means the buffer is empty and all the write ends have closed.
        if (bytes_read == 0) {
            if (count > 0) {
                read_buffer[count] = '\0';
                fprintf(stdout, "Child Recieved: %s\n", read_buffer);
            }
            fprintf(stdout, "Child: EOF reached.\n");

        } else if (bytes_read < 0) {
            // Check if read() failed.
            perror("read");
        }

        // Close pipefd[0] because child is done reading.
        close(pipefd[0]);
        fflush(stdout);
        _exit(EXIT_SUCCESS);

    } else {
        // Parent Process

        // Close pipefd[0] (read endpoint) because the parent will not be reading in this case.
        close(pipefd[0]);

        char message1[] = "Hello Child!\n";
        char message2[] = "How have you been!\n";
        char message3[] = "Goodbye Child!\n";

        /*
            Note: Write can sometimes fail or only write part of a message. If
            write() only writes part of a message you need to make sure to write the rest.
        */
        write(pipefd[1], message1, strlen(message1));
        write(pipefd[1], message2, strlen(message2));
        write(pipefd[1], message3, strlen(message3));

        // Close pipefd[1] (write endpoint) because the parent is done writing.
        close(pipefd[1]);

        int status;
        pid_t waited_child = waitpid(pid, &status, 0);
        // Check if wait failed.
        if (waited_child == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Check if the waited child is the one the parent spawned.
        if (waited_child == pid) {
            if (WIFEXITED(status)) {
                fprintf(stdout, "Parent: Child Exit Code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(stdout, "Parent: Child Killed by Signal: %d\n", WTERMSIG(status));
            }
        }
    }

    return 0;
}
```

## 5. Child to Child - Unidrectional (One Way Communication)
In this case Child 1 will write and Child 2 will read.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        // Always check if pipe() failed.
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        // Always check if fork() failed.
        perror("fork1");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        // Child 1 Process

        // Close pipefd[0] since Child 1 will not be reading.
        close(pipefd[0]);

        char message1[] = "Hello Child 2, This is Child 1.\n";
        char message2[] = "I'm the first Child.\n";
        char message3[] = "Goodbye.\n";

        write(pipefd[1], message1, strlen(message1));
        write(pipefd[1], message2, strlen(message2));
        write(pipefd[1], message3, strlen(message3));

        // Close pipefd[1] since Child 1 is done writing.
        close(pipefd[1]);

        _exit(EXIT_SUCCESS); // Terminate the Child
    }

    /*
        Since we will make Child 1 write and Child 2 read,
        Child 2 does not need to inherit pipefd[1] (the write end),
        therefore you can just close it in the parent.
    */
    // Close the pipefd[1] since Parent does not need to write and niether does Child 2.
    close(pipefd[1]);

    pid_t pid2 = fork();
    if (pid2 < 0) {
        // Always check if fork() failed.
        perror("fork2");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {
        // Child 2 Process
        // Since we already closed pipefd[1] in the parent, pipefd[1] was not inherited by Child 2
        char character;
        char read_buffer[1024];
        ssize_t bytes_read;
        unsigned int count = 0;
        while ((bytes_read = read(pipefd[0], &character, 1)) > 0) {
            if (character == '\n') {
                read_buffer[count] = '\0';
                count = 0;
                fprintf(stdout, "Child 2 Recieved: %s\n", read_buffer);
            } else {
                if (count >= sizeof(read_buffer) - 1) {
                    fprintf(stderr, "Child 2: Message too long!\n");
                    _exit(EXIT_FAILURE);
                }
                read_buffer[count++] = character;
            }
        }

        // Check if we reached EOF, this means the buffer is empty and all the write ends have closed.
        if (bytes_read == 0) {
            if (count > 0) {
                read_buffer[count] = '\0';
                fprintf(stdout, "Child 2 Recieved: %s\n", read_buffer);
            }
            fprintf(stdout, "Child: EOF reached.\n");

        } else if (bytes_read < 0) {
            // Check if read() failed.
            perror("read");
        }

        // Close pipefd[0] because Child 2 is done reading.
        close(pipefd[0]);
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }

    // Close pipefd[0] in Parent because the Parent does not need to read in this case.
    close(pipefd[0]);

    // You can make your wait more robust if you want instead of just wait(NULL).
    wait(NULL); // Wait for any one child
    wait(NULL); // Wait for the other one child.

    return 0;
}
```

## 6. Parent to Child - Bidirectional (Two Way Communication)
If one pipe allows one way communication, then two pipes allows two way communication. In this case the Parent and the Child can talk back and forth, so they can both read and write.
```
+------------+      +----------------------+      +------------+
[ pipe1fd[1] ]      [         Pipe1        ]      [ pipe1fd[0] ]
[ Write End  ] ---> [ Circular Byte Buffer ] ---> [  Read End  ]
+------------+      +----------------------+      +------------+
Parent Process                                     Child Process
+------------+      +----------------------+      +------------+
[ pipe2fd[0] ]      [         Pipe2        ]      [ pipe2fd[1] ]
[  Read End  ] <--- [ Circular Byte Buffer ] <--- [  Write End ]
+------------+      +----------------------+      +------------+
```
In this case we want the parent to send the child a string, the child will process the string by making it upper case and sending it back to the parent.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_WORD_LEN 80
#define MAX_WORDS 10

// Function to uppercase a word.
void uppercase_word(char *word) {
    if (word == NULL) {
        return;
    }
    for (size_t i = 0; word[i] != '\0'; i++) {
        word[i] = (char)toupper((unsigned char)word[i]);
    }
}

int main(void) {
    int pipe1fd[2];
    if (pipe(pipe1fd) < 0) {
        // Always check if pipe() failed.
        perror("pipe1");
        exit(EXIT_FAILURE);
    }

    int pipe2fd[2];
    if (pipe(pipe2fd) < 0) {
        // Always check if pipe() failed.
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork");
        exit(EXIT_FAILURE);

    }  else if (pid == 0) {
        // Child Process
        close(pipe1fd[1]); // Child is writing to pipe2fd[1], so close pipe1fd[1].
        close(pipe2fd[0]); // Child is reading from pipe1fd[0], so close pipe2fd[0].

        char character;
        char read_buffer[MAX_WORD_LEN];
        ssize_t bytes_read;
        unsigned int count = 0;
        // Keep Reading bytes sent by Parent
        while ((bytes_read = read(pipe1fd[0], &character, 1)) > 0) {
            if (character == '\0') {
                read_buffer[count] = '\0';
                count = 0;
                uppercase_word(read_buffer);
                // Send Result back to Parent.
                write(pipe2fd[1], read_buffer, strlen(read_buffer) + 1);
            } else {
                if (count >= sizeof(read_buffer) - 1) {
                    fprintf(stderr, "Word too long");
                    _exit(EXIT_FAILURE);
                }
                read_buffer[count++] = character;
            }
        }

        close(pipe1fd[0]); // Child is done reading, close pipe1fd[0].
        close(pipe2fd[1]); // Child is done writing, close pipe2fd[1].

        _exit(EXIT_SUCCESS);
    } else {
        // Parent Process
        close(pipe1fd[0]); // Parent is reading from pipe2fd[0], so close pipe1fd[0]
        close(pipe2fd[1]); // Parent is writing to pipe1[1], so close pipe2fd[1]

        char word1[] = "hello";
        char word2[] = "world";
        char word3[] = "GoOdByE";

        char *words[] = {word1, word2, word3};
        size_t num_words = sizeof(words) / sizeof(words[0]);

        char processed_words[MAX_WORDS][MAX_WORD_LEN];

        for (size_t i = 0; i < num_words; i++) {
            fprintf(stdout, "Before Processed Word %zu: %s\n", i, words[i]);

            // Write the word to be processed by the Child.
            write(pipe1fd[1], words[i], strlen(words[i]) + 1);

            // Read the result sent by the Child.
            read(pipe2fd[0], processed_words[i], MAX_WORD_LEN);

            fprintf(stdout, "After Processed Word %zu: %s\n\n", i, processed_words[i]);
            fflush(stdout);
        }

        close(pipe1fd[1]); // Parent is done writing, close pipefd[1].
        close(pipe2fd[0]); // Parent is done reading, close pipe2fd[0].

        // Don't forget to wait for the child.
        wait(NULL);
    }

    return 0;
}
```

**Note:** You can also do this with two Child processes where Child 1 and Child 2 can communicate back and forth.

## 7. Chaining Processes
Imagine you want to run a program and you want to pass the output as input to the next program. That is essentially the `|` operator used in commands like `echo 'Banana\napple\nbanana\ncitrus\nApple' | tr A-Z a-z | sort | uniq` where the output of the previous command is passed as input for the next command.

Lets try doing it for: `echo 'Banana\napple\nbanana\ncitrus\nApple' | tr A-Z a-z | sort | uniq`

Below is a diagram basically showing how we will implement this in C.
```
[Program 1]        [Program 2]       [Program 3]       [Program 4] -> stdout
         |          ^       |         ^       |         ^
         v          |       v         |       v         |
        [(1) pipe1 (0)]   [(1) pipe2 (0)]   [(1) pipe3 (0)]
```

C Program
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    /*====================
        Run Command 1
    ====================*/
    int pipefd[2];

    // Initialize Pipe 1
    if (pipe(pipefd) < 0) {
        // Always check if pipe() failed.
        perror("pipe1");
        exit(EXIT_FAILURE);
    }
    // Keep track of the previous pipe read endpoint.
    // That is where the next process will read the output of the previous process
    int previous_read_fd = -1;

    // Spawn Child 1
    pid_t pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork1");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child Process 1

        // Close pipefd[0], this Child not need to read from this end.
        close(pipefd[0]);

        /*
            By default the program will output to standard out (1) so redirect
            standard out (1) to the pipe's write end (pipefd[1]).
        */
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need pipefd[1] after we redirected standard out (1) to write end (pipefd[1]).
        close(pipefd[1]);

        // Run the program.
        char *command1[] = {"echo", "Banana\napple\nbanana\ncitrus\nApple", NULL};
        execvp(command1[0], command1);

        // Should not reach here unless exec failed.
        perror("exec");
        _exit(EXIT_FAILURE); // Terminate the Child
    }

    close(pipefd[1]);             // Close pipefd[1], Parent not writing anything.
    previous_read_fd = pipefd[0]; // Update the the previous pipe read endpoint.

    /*====================
        Run Command 2
    ====================*/

    // Initialize Pipe 2
    if (pipe(pipefd) < 0) {
        // Always check if pipe() failed.
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    // Spawn Child 2
    pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork2");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child Process 2

        // Again, close pipefd[0], this Child not need to read from this end.
        close(pipefd[0]);
        // Redirect standard in (0) to `previous_read_fd` (the previous pipe read endpoint).
        if (dup2(previous_read_fd, STDIN_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need `previous_read_fd` after we redirected standard in (0) to `previous_read_fd`.
        close(previous_read_fd);

        // Redirect standard out (1) to pipefd[1].
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need pipefd[1] after we redirected standard out (1) to write end (pipefd[1]).
        close(pipefd[1]);

        // Run the program.
        char *command2[] = {"tr", "A-Z", "a-z", NULL};
        execvp(command2[0], command2);

        // Should not be here unless exec failed.
        perror("exec");
        _exit(EXIT_FAILURE);
    }

    close(pipefd[1]);             // Close pipefd[1], Parent not writing anything.
    close(previous_read_fd);      // Close the old previous pipe read endpoint.
    previous_read_fd = pipefd[0]; // Update the the previous pipe read endpoint to the new one.

    /*====================
        Run Command 3
    ====================*/

    // Initialize Pipe 3
    if (pipe(pipefd) < 0) {
        // Always check if pipe() failed.
        perror("pipe3");
        exit(EXIT_FAILURE);
    }

    // Spawn Child 3
    pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork3");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // Child Process 3

        // Again, close pipefd[0], this Child not need to read from this end.
        close(pipefd[0]);
        // Redirect standard in (0) to `previous_read_fd` (the previous pipe read endpoint).
        if (dup2(previous_read_fd, STDIN_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need `previous_read_fd` after we redirected standard in (0) to `previous_read_fd`.
        close(previous_read_fd);

        // Redirect standard out (1) to pipefd[1].
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need pipefd[1] after we redirected standard out (1) to write end (pipefd[1]).
        close(pipefd[1]);

        // Run the program.
        char *command3[] = {"sort", NULL};
        execvp(command3[0], command3);

        // Should not be here unless exec failed.
        perror("exec");
        _exit(EXIT_FAILURE);
    }

    close(pipefd[1]);             // Close pipefd[1], Parent not writing anything.
    close(previous_read_fd);      // Close the old previous pipe read endpoint.
    previous_read_fd = pipefd[0]; // Update the the previous pipe read endpoint to the new one.

    /*====================
        Run Command 4
    ====================*/

    // Spawn Child 4
    pid = fork();
    if (pid < 0) {
        // Always check if fork() failed.
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child Process

        // Redirect standard in (0) to `previous_read_fd` (the previous pipe read endpoint).
        if (dup2(previous_read_fd, STDIN_FILENO) == -1) {
            // Always check if dup failed.
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        // No longer need `previous_read_fd` after we redirected standard in (0) to `previous_read_fd`.
        close(previous_read_fd);

        // Run the program.
        char *command4[] = {"uniq", NULL};
        execvp(command4[0], command4);

        // Should not be here unless exec failed.
        perror("exec");
        _exit(EXIT_FAILURE);
    }

    // Close the old previous pipe read endpoint.
    close(previous_read_fd);

    // The Parent spawned 4 child, so it needs to wait for 4 of them.
    // You can make the wait more robust instead of just `wait(NULL)`
    /*
        You might also be wondering, why not wait everytime before the start of the next
        program or process? Remember the process can write to the pipe buffer as
        long as there is space, but what if that space runs out while a read endpoint
        is still open? It blocks and waits for some other process to read the from
        buffer to make more space. However, remember that wait is also a blocking
        call, since the parent is blocking and the child is also blocking you created a
        deadlock and no one can do anything, essentually they are stuck waiting for each
        other.
    */
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
```
**Note:** That was pretty long, but you probably realized that you could of written that with a loop.

### Example: Just to Show the Deadlock Problem with Wait.
Take a look at the previous example before looking at this one, especially the wait section.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Spawn Child 1
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child Process 1
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        close(pipefd[1]);

        // Outputs integer from 1 to 1000000, very big stuff.
        char *command1[] = {"seq", "1", "1000000", NULL};
        execvp(command1[0], command1);

        perror("exec");
        _exit(EXIT_FAILURE);
    }

    close(pipefd[1]);
    // !!! This time we wait before the next process or program.
    /*
        What should happen here:
            - Parent waiting for Child 1 to terminate (Blocks).
            - Child 1 is waiting for more space in the pipe buffer (Blocks).
            - Deadlock. Everyone is waiting for each other nothing happens.
        Your terminal currently is probably showing that your program is
        still running but nothing is happening.
    */
    wait(NULL);

    // !!! This part won't even run.
    // Spawn Child 2
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child 2
        if (dup2(pipefd[0], STDIN_FILENO) < 0) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }
        close(pipefd[0]);

        char *command2[] = {"wc", NULL};
        execvp(command2[0], command2);

        perror("exec");
        _exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    wait(NULL);

    return 0;
}
```

## 8. Important Stuff

| File Descriptor | Meaning |
|---|---|
| `STDIN_FILENO` (0) | Standard Input |
| `STDOUT_FILENO` (1) | Standard Output |
| `STDERR_FILENO` (2) | Standard Error |
| `pipefd[0]` | Pipe Read End |
| `pipefd[1]` | Pipe Write End |

To redirect standard output to a pipe write endpoint.
```c
dup2(pipefd[1], STDOUT_FILENO);
```

To redirect standatd input to a pipe read endpoint.
```c
dup2(pipefd[0], STDIN_FILENO);
```

Other Notes
1. Please always remember to close any pipe endpoints as soon as they become unnecessary, very very Important.
2. Make sure to not create a deadlock situation when using wait and pipes.
3. 1 pipe for unidirectional (one way communication), 2 pipes for bidirection (two way communication).
4. You need N - 1 pipes for N chained processes or programs.
5. Redirect before exec.
6. If you confused what to redirect to what, or what pipe end the process read or write from, draw a diagram. It is very helpful.