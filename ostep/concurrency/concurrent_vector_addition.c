#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int* a;
    int* b;
    int* out;
    int start;
    int end;
} ThreadArgs;

void* vector_addition_thread(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    for (int i = thread_args->start; i < thread_args->end; ++i) {
        // printf("Adding %d + %d...\n", thread_args->a[i], thread_args->b[i]);
        thread_args->out[i] = thread_args->a[i] + thread_args->b[i];
    }
    return NULL;
}

void arange(int* arr, int size) {
    for (int i = 0; i < size; ++i) {
        arr[i] = i;
    }
}

void print_arr(int* arr, int size) {
    if (size == 0) {
        printf("{ }");
    }

    if (size == 1) {
        printf("{ %d }", arr[0]);
    }

    printf("{ %d, ", arr[0]);
    for (int i = 1; i < size - 1; ++i) {
        printf("%d, ", arr[i]);
    }
    printf("%d }\n", arr[size-1]);
}

double single_core_vec_add(int* a, int* b, int* out, int size) {
    clock_t start = clock();
    for (int i = 0; i < size; ++i) {
        out[i] = a[i] + b[i];
    }
    clock_t end = clock();
    double total = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Single-core time: %f seconds\n", total);
    return total;
}

double multi_core_vec_add(int* a, int* b, int* out, int numCores, int size) {
    pthread_t threads[numCores];
    ThreadArgs thread_args[numCores];

    int chunkSize = (size / numCores);
    clock_t start = clock();
    for (int i = 0; i < numCores; ++i) {
        thread_args[i].a = a;
        thread_args[i].b = b;
        thread_args[i].out = out;
        thread_args[i].start = i * chunkSize;
        thread_args[i].end = (i == numCores - 1) ? size : (i + 1) * chunkSize;
        pthread_create(&threads[i], NULL, vector_addition_thread, &thread_args[i]);
    }

    for (int i = 0; i < numCores; ++i) {
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();
    double total = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Multi-core time: %f seconds\n", total);
    return total;
}

int main () {
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);
    if (numCores > 0) {
        printf("Total number of cores: %ld\n", numCores);
    } else {
        perror("sysconf");
    }

    int size = 10000;
    int* a = (int*) malloc(size * sizeof(int));
    arange(a, size);

    int* b = (int*) malloc(size * sizeof(int));
    arange(b, size);

    int* single_core_out = (int*) malloc(size * sizeof(int));
    int* multi_core_out = (int*) malloc(size * sizeof(int));

    clock_t start, end;
    double single_core_time, multi_core_time;

    // Run single core vector addtion
    single_core_time = single_core_vec_add(a, b, single_core_out, size);

    // Run multi core vector addition
    multi_core_time = multi_core_vec_add(a, b, multi_core_out, numCores, size);

    double speedup = single_core_time / multi_core_time;
    printf("Speedup: %f\n", speedup);

    // print_arr(single_core_out, size);
    // print_arr(multi_core_out, size);

    free(a);
    free(b);
    free(single_core_out);
    free(multi_core_out);

    return 0;
}