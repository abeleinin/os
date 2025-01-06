#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void run(char* cmd) {
  char *args[2];
  args[0] = cmd;
  args[1] = NULL;
  execvp(cmd, args);
}

void closepipes(int *fds) { 
  close(fds[0]);
  close(fds[1]);
}

int main(int argc, char *argv[]) {
  int filedes[2];
  pipe(filedes);
  pid_t pid_a, pid_b;

  if (!(pid_a = fork())) {
    // child 1
    dup2(filedes[1], 1);
    closepipes(filedes);
    run("ls");
  } 
    
  if (!(pid_b == fork())) {
    // child 2
    dup2(filedes[0], 0);
    closepipes(filedes);
    run("sort");
  }

  // parent path
  closepipes(filedes);
  waitpid(pid_a, NULL, 0);
  waitpid(pid_b, NULL, 0);
  return 0;
}

