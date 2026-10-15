# FILE IO

## 1. File IO - High Level

High level file IO is provided by the standard c libary `<stdio.h>` that abstracts the low level file IOs. Instead of interacting with file descriptors, you work with streams represented by `FILE *`. The File object is not the file itself and is instead managed by the library to interact with the actual file. It is a lot more convenient than low level file IO.

The FILE object has information like:
- File descriptor
- IO buffer
- Current stream position
- End of file indicator

### High Level IO Functions

```c
#include <stdio.h>

// Open and close files.
FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);

// Character IO
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);

// String IO
char *fgets(char *buf, int buf_size, FILE *stream);
int fputs(const char *str, FILE *stream);

// Formatted IO
int fscanf(FILE *stream, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);

// Block IO - Readings data as chunks of bytes
size_t fread(void *ptr, size_t size, size_t nmeb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmeb, FILE *stream);

// Positioning
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
```

### Default File Streams
1. Standard Input: `stdin`
2. Standard Output: `stdout`
3. Standard Error: `stderr`

## 1.1 High Level File IO - fopen() and fclose()

```c
#include <stdio.h>

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
```

| function| Description | Parameters | Return |
| :--- | :--- | :--- | :--- |
| `fopen`| Open a file by filename in a certain mode. | <ol><li>filename - The name of the file.</li><li>mode - how you want to open the file.</li> | <ul><li>**On Success** - Return pointer to `FILE` stream object.</li><li>**On Failure** - Return `NULL`.|
| `fclose` | Close an existing open `FILE` stream. | <ol><li>stream - The previously opened file stream. </li> | <ul><li>**On Success** - Return `0`.</li><li>**On Failure** - Return `EOF` </li></ul> |

### File Open Modes
| Mode | Read | Write | If File Exists? | If File Does Not Exists? | Initial Position |
|---|---|---|---|---|---|
| `r` | Yes | No | Open normally | Return `NULL` | Start of file |
| `r+` | Yes | Yes | Open normally | Return `NULL` | Start of file |
| `w` | No | Yes | Truncate to 0 length (erase all data) | Creates new file | Start of file |
| `w+` | Yes | Yes | Truncate to 0 length (erase all data) | Creates new file | Start of file |
| `a` | No | Yes | Appends to end | Creates new file | End of file |
| `a+` | Yes | Yes | Appends to end | Creates new file | End of file for write, for read it depends on the system. On unix, the read starts at the beginning of the file. |

## 1.2 High Level File IO - fgetc() and fputc()
```c
#include <stdio.h>

int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
```
| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `fgetc` | Read a single character from a file stream and advance file position. | <ol><li>`stream` - The opened `FILE` object to read from. </li></ol> | <ul><li>**On Success** - Return the character read as an `int` type to represent all characters including `EOF` just convert it back `(unsigned char)`.</li><li>**On Failure** - Returns `EOF`</li></ul> |
| `fputc` | Write a single character to file stream and advance file position. | <ol><li>`c` - The character to write.</li><li>`stream` - The opened `FILE` object to write to. </li></ol> | <ul><li>**On Success** - Return the character value you just wrote.</li><li>**On Failure** - Return `EOF`</li></ul> |

## 1.3 High Level File IO - fgets() and fputs()
```c
#include <stdio.h>

char *fgets(char *buf, int buf_size, FILE *stream);
int fputs(const char *str, FILE *stream);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `fgets` | Read characters from a opened `FILE` stream and stores them as a string into a buffer until it hits a newline (`\n` is also read into the buffer),reaches end of file (`EOF`), or it safely fills `buf_size - 1` bytes. Advances file position by number of bytes read. | <ol><li>`buf` - pointer to the buffer that will store what is read. </li><li>`buf_size` - The size of the buffer in bytes.</li><li>`stream` - The opened `FILE` stream for reading. </li></ol> | <ul><li>**On Success** - return pointer to destination buffer.</li><li>**On Failure** - return `NULL`</li></ul> |
| `fputs` | Writes a null terminating string to a opened `FILE` stream. | <ol><li>`str` - The null terminating string to write.</li><li>`stream` - The `FILE` stream to write to.</li></ol> | <ul><li>**On Success** - Return non negative number.</li><li>**On Failure** - Return `EOF`.</li></ul> |

**Note:**
- `fgets` will always append a null terminating (`\0`) into your buffer as long as you give the correct buffer size.
- `fgets` will keep the newline (`\n`) in the buffer if it finds one in the `FILE` stream before it reads `buf_size - 1` bytes.
- `fputs` will use null terminating (`\0`) in your input string to know when to stop writing. It will not write null terminating (`\0`) to the `FILE` stream.
- `fputs` does not automatically add a newline (`\n`) at the end when it is done writing.

## 1.4 High Level File IO - fprintf() and fscanf()
```c
#include <stdio.h>

int fscanf(FILE *stream, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `fscanf` | Read formatted data from open `FILE` stream, parsing the text into variables using format specifiers (`%d`, `%c`, `%s`, and others). This means you need to pass the variable addresses. | <ol><li>`stream` - The open `FILE` stream to read from.</li><li>`format` - The formatter string to layout what to read.</li><li>`...` - Variable pointers (addresses), basically where to store the data parsed from the format specifiers.</li></ol> | <ul><li>**On Success** - Return the number of input items read successfully matched and then assigned.</li><li>**On Failure** - Return `EOF`</li></ul> |
| `fprintf` | Write formatted data to open `FILE` stream, convert variable values into specified strings.| <ol><li>`stream` - The open `FILE` stream to write to.</li><li>`format` - The formatter string to layout what to write.</li><li>`...` - Variables or values to write.</li></ol> | <ul><li>**On Success** - Return the total number of characters (bytes) written to the `FILE` stream.</li><li>**On Failure** - Return a negative number.</li></ul> |

**Note:**
- `fscanf` - If you use `%s` format specifier to scan or read a string into a buffer, it will automatically append a null terminating (`\0`) for you. However, beware that `fscanf` is not know how large the destination buffer or array is, for `%s` it only stops reading until it meets the first whitespace character. So it is only recommended to be used when you know the exact format of the file you want to read.

## 1.5 High Level File IO - fread() and fwrite()
```c
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nmeb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmeb, FILE *stream);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `fread` | Read specified chunks of bytes from an open `FILE` stream into a buffer. | <ol><li>`ptr` - Pointer to buffer to store data read.</li><li>`size` - The size of a single element in bytes in the buffer.</li><li>`nmeb` - The number of elements of that size you want to read.</li><li>`stream` - The open `FILE` stream to read from.</li></ol> | <ul><li>**On Success** - Return the exact number of `nmeb` elements requested to be read.<li>**On Failure** - Return less than `nmeb` due to `EOF` or other errors.</li></ul> |
| `fwrite` | Write specified chunks of bytes to a open `FILE` stream from a buffer. | <ol><li>`ptr` - Pointer to buffer with data to write.</li><li>`size` - The size of a single element in bytes in the buffer.</li><li>`nmeb` - The number of elements of that size you want to write.</li><li>`stream` - The open `FILE` stream to write to.</li></ol> | <ul><li>**On Success** - Return the exact number of `nmeb` elements requested to be written.<li>**On Failure** - Return less than `nmeb`, can be because disk is full or file permission issue. |

## 1.6 High Level File IO - Positioning
```c
#include <stdio.h>

int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `fseek` | Moves file position indicator to a specific location in the `FILE` stream, which allows non sequential (random) reading or writing. | <ol><li>`stream` - The open `FILE` stream.</li><li>`offset` - The number of bytes to move from the starting point.</li><li>`whence` - The starting reference point for the offset. It has to be one of the three constants: <ul><li>`SEEK_SET` - Start from beginning.</li><li>`SEEK_CUR` - Start from current position.</li><li>`SEEK_END` - Start from the end of the file (Past the last byte).</li></ul></li></ol> | <ul><li>**On Success** - Return `0`.</li><li>**On Failure** - Return non zero value usually `-1`.</li></ul> |
| `ftell` | Tells you how many bytes the current file position indicator is away from the beginning of the file.| <ol><li>`stream` - The open `FILE` stream.</li></ol> | <ul><li>**On Success** - Return the current byte offset from the beginning of the file in long type.</li><li>**On Failure** - Return `-1` in long type.</li></ul> |
| `rewind` | Reset file position indicator back to beginning of the file and clear any error or `EOF` flags. | <ol><li>`stream` - The open `FILE` stream.</li></ol> | <ul><li>No Return Value.</li></ul> |

---

## 2. File IO - Low Level
Low level IO communicate directly with the OS kernel by making system calls when you call the low level functions. Instead of using `FILE` streams, you use file descriptors that maps to an actual file.

**Note:** Making a system call is slow, so if you are writing or reading a lot of data in small chunks you will be making a lot of system calls that will slow down the program. This is why buffering existed with `FILE` streams so that you don't constantly make system calls and instead make one system call when the buffer is full.

### Low Level IO Functions
```c
#include <fcntl.h> // For open() and O_flags
#include <unistd.h> // For read(), write(), close(), and lseek()

// Open and close files.
int open(const char *pathname, int flags, /* mode_t mode */);
int close(int fd);

// Read and write data in bytes.
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

// Positioning.
off_t lseek(int fd, off_t offset, int whence);
```

### Default File Descriptors
1. Standard Input - `0`
2. Standard Output - `1`
3. Standard Error - `2`

## 2.1 Low Level File IO - open() and close()
```c
#include <fcntl.h>
#include <unistd.h>

int open(const char *pathname, int flags, /* mode_t mode */);
int close(int fd);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `open` | Establish a connection between the lowest available file descriptor and the actual file, so that you can interact with the actual file. | <ol><li>`pathname` - A null terminating string that contain the path to that file.</li><li>`flags` - bitwise mask that controls how the file is opened.</li><li>`mode` - Specify the file permissions, this only applies when creating the new file.</li></ol> | <ul><li>**On Success** - Return lowest available non negative file descriptor.</li><li>**On Failure** - Returns `-1` and sets global `errno`.</li></ul> |
| `close` | Release the file descriptor so that it is no longer used. | <ol><li>`fd` - The valid open integer file descriptor that needs to be closed.</li></ol> | <ul><li>**On Success** - Return `0`.</li><li>**On Failure** - Return `-1` and sets global `errno`.</li></ul> |

### Important open() flags
| Flag | Description |
|---|---|
| `O_RDONLY` / `O_WRONLY` / `O_RDWR` | You can only choose one access mode: read, write, or both. |
| `O_CREAT` | Create missing file. You will need to set file permission mode if you use this flag. |
| `O_TRUNC` | Truncate existing regular file, if you opened with with suitable write access.  |
| `O_APPEND` | Each write is appended to the file end. |
<!-- | `O_EXCL` | Paired with `O_CREAT`, does the checking and creating in one atomic step. |
| `O_CLOEXEC` | Close this file descriptor if a running process successfully calls an exec function. | -->

### File Permission - mode_t mode

If you use the `O_CREAT` flag you must set the file permission. The file permission can be represented by 4 digits such as: `0644`.
| Digit Position | For Who? |
|---|---|
| 1st Digit (0) | Octal - Interpret the following number as base-8 (octal) instead of base-10 (decimal).|
| 2nd Digit (6) | Owner / User - The one who owns the file. |
| 3rd Digit (4) | Group - The users who belong to the file's group. |
| 4th Digit (4) | Others - Everyone else in the system. |

### File Permission - How each Individual Digit is Calculated
Each digit is a single number from 0 to 7. The number is calculated by adding up the following:
- `4` = Read(`r`)
- `2` = Write(`w`)
- `1` = Execute(`x`)
- `0` = No Permission(`-`)

To get a specific combination, you just have to add the numbers together:
- `7` (4 + 2 + 1) = Read, Write and Execute (`rwx`)
- `6` (4 + 2 + 0) = Read and Write (`rw-`)
- `5` (4 + 0 + 1) = Read and Execute (`r-x`)
- `4` (4 + 0 + 0) = Read Only (`r--`)
- `3` (0 + 2 + 1) = Write and Execute (`-wx`)
- `2` (0 + 2 + 0) = Write Only (`-w-`)
- `1` (0 + 0 + 1) = Execute Only (`--x`)
- `0` (0 + 0 + 0) = No Permission

Exmaple: `0644` -> `rw-r--r--`. Owner gets read write access. Group gets only read access. Others also get only read access.


## 2.2 Low Level File IO - read() and write()
```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `read` | Read up to specified number of bytes from an open file descriptor into memory buffer. | <ol><li>`fd` - The open integer file descriptor to read from.</li><li>`buf` - A pointer to the memory buffer where the read data will be stored.</li><li>`count` - The maximum number of bytes to read into the buffer.</li></ol> | <ul><li>**On Success** - Return number of bytes actually read (non negative), sometimes can be fewer than the number of bytes specified and 0 if `EOF`.</li><li>**On Failure** - Return `-1` and set global `errno`.</li></ul> |
| `write` | Write up to specified number of bytes from a memory buffer to an open file descriptor. | <ol><li>`fd` - The open file descriptor to write to.</li><li>`buf` - A pointer to the memory buffer containing the data to be written. </li><li>`count` - The number of bytes to write from the buffer.</li></ol> | <ul><li>**On Success** - Return the number of bytes actually written (non negative), Sometimes can be fewer than the number of bytes specified due to near full disk or system call interruption.</li><li>**On Failure** - Return `-1` and set global `errno`.</li></ul> |

## 2.2 Low Level File IO - Postioning
```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

| Function | Description | Parameter | Return |
| :--- | :--- | :--- | :--- |
| `lseek` | Reposition the read or write file offset associated with a file descriptor to a new location | <ol><li>`fd` - The open integer file descriptor you want to change the offset of.</li><li>`offset` - The number of bytes to move relative to the start position.</li><li>`whence` - The start position. It must be one of the three constants: <ul><li>`SEEK_SET` - Beginning of file.</li><li>`SEEK_CUR` - Current Location of file.</li><li>`SEEK_END` - End of file (past the last byte).</li></ul></li></ol> | <ul><li>**On Success** - Return the resulting file offset in bytes from the beginning of the file (non negative).`off_t` is an signed int.</li><li>**On Failure** - Returns `-1` and set global `errno`.</li></ul> |
