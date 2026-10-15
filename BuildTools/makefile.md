# Makefile Guide

Make is a build tool. You tell what files depend on what other files, and how to produce one file from another. When you run the `make` command, it builds non-existent targets or rebuilds out of date targets.

---

## 0. What Happens When You Compile

Make exiists to organize steps for a build. Basically it turns C source code into a runnable program, and it takes 4 stages.

1. **Preprocessing** - Handles all lines starting with `#`. Pastes full text of each `#include` header into your file, expands macros, and resolves `#ifdef` and `#ifndef` conditions.

2. **Compiling** - Compiler parses the translation unit (preprocesses block of code) and converts it to assembly code. It also makes sure that the code is following all the rules of the language including, checking syntax and types, enforcing scopes, and validating definitions(only one definition allowed for a specific function, struct, ...).

3. **Assembling** - Takes assembly code and converts it into binary object file (extension ends with .o or .obj)

4. **Linking** - Connects all object files together and produce a single executable.

## 1. Makefile Rules

Every Makefile is built out of **rules**. A rule looks like this:

```make
# This is a comment
target: prerequisites
    recipe
```

- **target** - usually the file you want to produce, though it can also be an action like `clean`.

- **prerequisites** - files the target depends on. If any of them are newer than the target (or the target doesn't exist yet), Make will run the recipe.

- **recipe** - commands that builds the target. **Must be indented with a literal TAB character**, not spaces.

Example:

```make
hello: hello.c
    gcc hello.c -o hello
```

If you run `make` with no argument, it will build the **first target in the makefile**. Run `make hello` to be more explicit. If `hello.c` has not changed, then rerunning does nothing.

---

## 2. Chains of rules

```make
app: main.o utils.o
    gcc main.o utils.o -o app

main.o: main.c utils.h
    gcc -c main.c -o main.o

utils.o: utils.c utils.h
    gcc -c utils.c -o utils.o
```

Prerequisites can be targets of other rules. Make will follow the chain down to files that already exist.

**Why is `utils.h` header file listed as a prerequisite?** Because main.c file `#includes` a custom `utils.h` header file. So if the `utils.h` file is changed we can run Make again to get the updated binary. If you don't include the `utils.h` and you changed the file, when you run Make, no new binary will be generated.

---

## 3. Phony (Fake) targets

Some targets are actions, not files (`clean`, `all`). If a file named `clean` ever exists, Make would think that target is "up to date" and skip it.

```make
# This prevents Make from thinking that `clean` is a file.
.PHONY: clean
clean:
    rm -f *.o app
```

---

## 4. Automatic variables

Shorthand inside a recipe.
| Var | Meaning |
|---|---|
| `$@` | the target |
| `$<` | first prerequisite |
| `$^` | all prerequisites |
| `$?` | prerequisites newer than the target |
| `$*` | the stem a pattern rule matched |


```make
app: main.o utils.o
    gcc $^ -o $@        # Link all prereqs into the target.

main.o: main.c utils.h
    gcc -c $< -o $@     # Compile the first prereq into the target.

utils.o: utils.c utils.h
    gcc -c $< -o $@
```

---

## 5. Variables

```make
CC = gcc
CFLAGS = -g -Wall -std=c99
```

Variables in Makefiles are basically placeholders, instead of typing gcc and the optiions many times, you can save time by putting them into variables. Later when you want to use them you use `$(VARIABLE_NAME)`, `$()` replaces the variable name with its actual value.

---

## 6. Assignment Operators

| Operator | Behavior | Use Case |
|---|---|---|
| `=` | This operator does not look at the value right away. It waits until the absolute last second to look up what the value of the variable is. | You can use variables that you haven't even created yet. |
| `:=` | This operator takes permanent snapshot of the value right now. Behaves like normal variables in normal programming. | It is predictable and fast. It doesn't look at anything defined later. |
| `?=` | This only sets the variable if it is completely empty, it will be ignored if a value is already set. | It lets the you override the Makefile from the terminal. |
| `+=` | This takes whatever is already in the variable and appends new text onto the end | You can build a long list of settings step by step. |


### Examples

`=`
```make
ANIMAL = $(SOUND) cat   # Make doesn't know what SOUND is yet, but it doesn't care.
SOUND = Meowing
# When you finally use $(ANIMAL) later, Make looks up both and prints: "Meowing cat"
```

`=:`
```make
SOUND = Meowing
ANIMAL := $(SOUND) cat  # Snapshot taken, ANIMAL is now "Meowing cat".
SOUND = Barking         # Changing SOUND later does NOT change ANIMAL.
```

`?=`
```make
CC ?= gcc
# If you only type `make`, it uses `gcc`. If you type `make CC=clang` in the terminal, Make will use clang and not gcc.
```

`+=`
```make
CFLAGS := -Wall
CFLAGS += -std=c17 # CFLAGS is now "-Wall -std=c17"
```

## 7. Wildcards

| Wildcard | Use Case |
|---|---|
| `*` | Used to find actual files on your computer. (Used in variables or recipes) |
| `%` | Used to define general rules by matching text patterns. (Used in target/prequisites) |

## 8. Static Rule Pattern

```bash
$(TARGETS): target-pattern : prereq-pattern
    recipe
```

- **TARGETS** - A list of specific file names that this rule applies to.
- **target-pattern** - A pattern continins a percent sign(%) that matches the stem of each target name.
- **prereq-patterns** - Patterns used to build the prerequisite names for each target by substituting the matching stem