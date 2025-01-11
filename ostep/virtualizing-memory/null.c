#include <stdio.h>

int main() {
  int* x = NULL;  

  printf("Seg fault: %d\n", *x);
}
