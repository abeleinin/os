#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char* argv[]) {
    struct timeval start, end;

    int PAGESIZE = atoi(argv[1]);
    int NUMPAGES = atoi(argv[2]);

    // Allocate memory aligned to page size to ensure proper TLB behavior
    int* a;
    if (posix_memalign((void **)&a, PAGESIZE, NUMPAGES * PAGESIZE) != 0) {
        perror("Memory allocation failed");
        return 1;
    }

    int jump = PAGESIZE / sizeof(int);
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUMPAGES * jump; i += jump) {
        a[i] += 1;
    }
    gettimeofday(&end, NULL);

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time: %.6f seconds\n", elapsed_time);

    return 0;
}