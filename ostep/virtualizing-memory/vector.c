#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct {
  size_t size;
  size_t capacity;
  int*   data;
} vector;

vector* create_vector(size_t n) {
  vector* vec = (vector*) malloc(sizeof(vector));

  vec->size = n;
  vec->capacity = 2*n;
  vec->data = (int*) calloc(2*n, sizeof(int));

  return vec;
}

vector* arange(int n) {
  vector* vec = create_vector(n);
  for (int i = 0; i < n; ++i) {
    vec->data[i] = i;
  }
  return vec;
}

vector* init_vector(int n, ...) {
  vector* vec = create_vector(n);

  va_list list;

  va_start(list, n);

  for (int i = 0; i < n; ++i) {
    vec->data[i] = va_arg(list, int);
  }

  va_end(list);

  return vec;
}

void destroy_vector(vector* vec) {
  if (vec != NULL) {
    free(vec->data);
    free(vec);
  }
}

void push_back(vector* vec, int val) {
  vec->size += 1;

  if (vec->size >= vec->capacity) {
    vec->data = realloc(vec->data, vec->capacity*2);
    vec->capacity *= 2;
  }

  vec->data[vec->size-1] = val;
}

void print_vector(vector* vec) {
  printf("[");
  for (int i = 0; i < vec->size-1; ++i) {
    printf("%d, ", vec->data[i]);
  }
  printf("%d]\n", vec->data[vec->size-1]);
}

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);

  printf("Test create_vector:\n");
  vector* v1 = create_vector(n);
  print_vector(v1);

  printf("Test arange:\n");
  vector* v2 = arange(n);
  print_vector(v2);

  printf("Test init_vector:\n");
  vector* v3 = init_vector(5, 9, 8, 7, 6, 5);
  print_vector(v3);

  printf("Test push_back on v3:\n");
  push_back(v3, 4);
  print_vector(v3);
  push_back(v3, 3);
  print_vector(v3);
  push_back(v3, 2);
  print_vector(v3);

  destroy_vector(v1);
  destroy_vector(v2);
  destroy_vector(v3);
}
