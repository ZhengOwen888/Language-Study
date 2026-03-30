# gdb Fundamentals

## Table of Contents
> - [0. Summary](#0.-summary)
> - [1. Starting gdb](#1.-starting-gdb)
> - [2. Breakpoints](#2.-breakpoints)
> - [3. Stepping](#3.-stepping)
> - [4. Inpecting Variables](#4.-inpecting-variables)
> - [5. Stack Tracing](#5.-stack-tracing)
> - [6. Coredumps](#6.-coredumps)
> - [7. References](#7.-references)
---

## 0. Summary
This document is meant for a quick reference for using **GDB** to debug C/C++ programs. It covers essential commands for:

- Starting GDB and running programs
- Setting and managing breakpoints
- Stepping through code
- Inspecting variables and their values
- Examining the function call stack
- Using core dumps for analysis

## 1. Starting gdb
To start gdb, make sure you have a executable c file.
```sh
gcc -Wall -g sumall.c -o sumall # compile the c program
gdb -tui sumall                 # gdb followed by executable name
```

## 2. Run
Run the program with the run command. Pass in command line arguments as arguments for the run command.

```sh
(gdb) run arg1 arg2 ...
```

## 3. Breakpoints
You can add breakpoints to stop the execution at a certain point in the program.

```sh
(gdb) break main    # set break at beginning of main() function.
(gdb) break 5       # set break at line 5 of current file.
(gdb) break sum.c:5 # set break at line 5 of sum.c

(gdb) info breakpoints # list all current breakpoints

(gdb) clear main # you can clear a breakpoint by function name or line number
(gdb) delete 1   # you can also clear a breakpoint by their number (the order they were created).
(gdb) enable 2   # enable followed by the breakpoint number.
(gdb) disable 3  # diable followed by the breakpoint number.
```

## 4. Stepping
To move around the program you can use the following commands.

```sh
(gdb) next      # next statement in the program.
(gdb) step      # step into a function call.
(gdb) continue  # continue to the next breakpoint that was set.
```

## 5. Inspecting Variables

If you want to display the values of variables during the run that are also in scope, you can use the following commands.

```sh
(gdb) display total # add the variable to the display list, which will be printed after gdb stops execution.
(gdb) display info  # display the list of variable to be printed with each having a number as label.
(gdb) undisplay 1   # use those number labels to remove a variable from the list.

(gdb) print total                 # print one variable.
(gdb) printf "Total: %d\n", total # formatted print.

(gdb) info locals # display all local variables.
```

## 6. Stack Tracing
If you want to look at the current function call stack use this command.
```sh
(gdb) backtrace # look at the current function call stack.
(gdb) finish    # execute the remainder of the current function and stops immediately after it returns.
(gdb) advance sum # execute until the given location is reached (line number, function name, filename:line).
```

## Coredumps
Segmentation fault occurs when the program accesses memory that it is not allowed to. Segment comes from memory segments (e.g. Text Segment, Data Segment, Heap, Stack)<br>.

When segmentation fault occurs, a snapshot of the program called core is created.
Use gdb examine the snapshot right before the program causes a segmentation fault.
```sh
./sumall
Segmentation fault (core dumped)
gdb -tui -c core sumall
```

If no core was created allow core dump of any size with this command.
```sh
ulimit -c unlimited
./sumall
```

## 7. References
1. <a href="https://beej.us/guide/bggdb/" target="_blank">Beej's Quick Guide to GDB</a>
