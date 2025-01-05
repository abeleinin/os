#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int x;
  printf("Type a number: ");
  scanf("%d", &x);

  int rc = fork();
  if (rc < 0) {
    // fork failed
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    // child (new process)
    printf("child (pid:%d)\n", (int) getpid());
    x = 1;
    printf("value of x: %d\n", x);
  } else {
    // parent path
    printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    printf("value of x: %d\n", x);
  }
  return 0;
}

