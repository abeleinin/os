#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  struct timeval start, end;

  int fd = open("/dev/null", O_RDONLY);

  gettimeofday(&start, NULL);
  for (int i = 0; i < 1000000; i++) {
    read(fd, NULL, 0);
  }

  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
  printf("Avg. read() syscall time: %f μs\n", elapsed / 1000000);

}