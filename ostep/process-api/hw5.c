#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int rc = fork();
  if (rc < 0) {
    // fork failed
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    // child (new process)
    printf("child (pid:%d)\n", (int) getpid());
    // int rc_wait = wait(NULL);
    // printf("Hello!, rc_wait:%d\n", rc_wait);
  } else {
    // parent path
    int rc_wait = wait(NULL);
    printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    printf("Goodbye!, rc_wait:%d\n", rc_wait);
  }
  return 0;
}

