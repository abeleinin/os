#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int fd;
  fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

  int rc = fork();

  if (rc < 0) {
    // fork failed
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    // child (new process)
    printf("child (pid:%d)\n", (int) getpid());
    int wr = write(fd, "Child Hello!\n", sizeof("Child Hello!\n") -1);
    if (wr < 0) {
      fprintf(stderr, "fork failed\n");
      close(fd);
      exit(1);
    }
  } else {
    // parent path
    printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    int wr = write(fd, "Parent Hello!\n", sizeof("Parent Hello!\n") -1);
    if (wr < 0) {
      fprintf(stderr, "fork failed\n");
      close(fd);
      exit(1);
    }
  }

  close(fd);
  return 0;
}

