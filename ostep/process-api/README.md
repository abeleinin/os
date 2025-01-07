# Process API

- [`fork()` system call](#fork-system-call)
- [`wait()` system call](#wait-system-call)
- [`exec()` system call](#exec-system-call)
- [Additional reading](#additional-reading)

## `fork()` system call

```c
// Advanced programming in the UNIX environment
// Chapter 8.3 
#include <unistd.h>
pid_t fork(void);
// Returns: 0 in child, process ID of child in parent, −1 on error
```

The value `fork()` returns depends on the caller. Specifically, the parent receives the PID of the newly-create child, but the child receives a return code of 0. PID 0 is reserved for use by the kernel, so it’s not possible for 0 to be the PID of a child.

```c
// Advanced programming in the UNIX environment
// Figure 8.1: Example of fork function

#include <stdio.h>
#include <unistd.h>

// external variable in initialized data
int globvar = 6;
char buf[] = "a write to stdout\n";

int main(void) {
  int     var;
  pid_t   pid;

  // automatic variable on the stack
  var = 88;
  if (write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1) {
    fprintf(stderr, "write failed\n");
  }

  // print before fork
  printf("before fork\n");

  if ((pid = fork()) < 0) {
    fprintf(stderr, "fork failed\n");
  } else if (pid == 0) {
    // modify variables in child
    globvar++;
    var++;
  } else {
    // parent
    sleep(2);
  }
  printf("pid = %ld, glob = %d, var = %d\n", (long) getpid(), globvar, var);
  return 0;
}
```

```txt
a write to stdout
before fork
pid = 235, glob = 7, var = 89
pid = 233, glob = 6, var = 88
```

The **child** process created by `fork()` has it's own private memory, its own registers, and its own program coutner (PC). Any modification to global/shared variables from the child will not update their value in the parent. 

```c
// OSTEP
// Figure 5.1: Calling fork() (p1.c)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  printf("hello (pid:%d)", (int) getpid());
  int rc = fork();
  if (rc < 0) {
    // fork failed fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    // child (new process)
    printf("child (pid:%d)\n", (int) getpid());
  } else {
    // parent path
    printf("parent of %d (pid:%d)\n", rc, (int) getpid());
  }
  return 0;
}
```

The `fork()` system call is used to create a new process. It creates an (almost) *exact copy of the calling process*. The **child** process comes into life as if it had called `fork()` itself. The output of `p1.c` is not **deterministic** and is dependent on the CPU scheduler policy.

- **Process Identifier (or PID)** is used to name the process, so the user can target it with a command.

- **CPU Scheduler** determines which process runs at a given moment in time.

## `wait()` system call

```c
// Advanced programming in the UNIX environment
// Chapter 8.6 
#include <sys/wait.h>
pid_t wait(int *statloc);
pid_t waitpid(pid_t pid, int *statloc, int options);
// Both return: process ID if OK, 0 (see later), or −1 on error
```

- The `wait` function can block the caller until  a child process terminates, whereas `waitpid` has an option that prevents it from blocking.
- The `waitpid` function doesn't wait for the child that terminates first; it has a number of options taht control which process it waits for.

`statloc` is a pointer to an integer, which specifies the termination status of the terminated process.

If it's required that the child and parent synchronize their actions, some form of interprocess communication is required.

## `exec()` system call

Useful when you want to run a program that's different from the calling program.  `exec` merely replaces the current process with a brand-new program from disk. The heap, stack and other parts of the memory space are re-initializaed. In many UNIX system implementations, only one of these seven functions, execve, is a system call within the kernel.

```c
// Advanced programming in the UNIX environment
// Chapter 8.10
#include <unistd.h>
int execl(const char *pathname, const char *arg0, ... /* (char *)0 */ );
int execv(const char *pathname, char *const argv[]);
int execle(const char *pathname, const char *arg0, ... /* (char *)0, char *const envp[] */ );
int execve(const char *pathname, char *const argv[], char *const envp[]); 
int execlp(const char *filename, const char *arg0, ... /* (char *)0 */ );
int execvp(const char *filename, char *const argv[]);
int fexecve(int fd, char *const argv[], char *const envp[]);
// All seven return: −1 on error, no return on success
```

- When *filename* arguement is specified:
  - If *filename* contains a slash, it is taken as a pathname.
  - Otherwise, the executable file is searched for in the directories specified by the `PATH` environment variable.
- The letter **p** means that the function takes a *filename* argument and uses the `PATH` environment variable to find the executable file
- The letter **l** means that the function takes a list of arguments and is mutually exclusive with the letter **v**
- The letter **v** means that it takes an argv[] vector
- The letter **e** means that the function takes an envp[] array instead of using the current environment

## Additional reading

- [Advanced programming in the UNIX environment](https://github.com/zwan074/technical-books/blob/master/Advanced.Programming.in.the.UNIX.Environment.3rd.Edition.0321637739.pdf)

- [A fork() in the road](https://www.microsoft.com/en-us/research/uploads/prod/2019/04/fork-hotos19.pdf)
