#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  long size = strtol(argv[1], NULL, 10);

  int n = (size * 1000000) / sizeof(int);

  int* arr;
  arr = (int*) malloc(size);

  if (arr == NULL) {
    printf("Memory allocation failed!\n");
    return 1;
  }

  for (int i = 0; i < n; i++) {
    printf("arr %d: %p\n", i, &arr[i]);
  }

  return 0;
}
