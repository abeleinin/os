#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define ITER_MAX 100000

int main(int argc, char* argv[]) {
  int pipe1[2], pipe2[2];
  pipe(pipe1);
  pipe(pipe2);

  char message = 'x';
  char buffer;

  pid_t pid = fork();

  if (pid == 0) {
    // Child process
    for (int i = 0; i < ITER_MAX; i++) {
      read(pipe1[0], &buffer, 1);
      write(pipe2[1], &message, 1);
    }
  } else {
    // Parent process
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < ITER_MAX; i++) {
      write(pipe1[1], &message, 1);
      read(pipe2[0], &buffer, 1);
    }

    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    printf("Total time for %d context switches: %f μs\n", ITER_MAX, elapsed);
    printf("Avg. context switch time: %f μs\n", elapsed / (2 * ITER_MAX));
  }
  return 0;
}
