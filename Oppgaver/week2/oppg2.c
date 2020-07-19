#include <stdlib.h>
#include <stdio.h>
#include <string.h> // memcpy
#include <time.h>

typedef double my_type; // Change double to int, short, long etc.

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Usage: ./prog.exe N_ELEMENTS");
        return 1;
    }
    int N = atoi(argv[1]);

    printf("Copying %d elements of type double.\n", N);

    my_type *a = malloc(N * sizeof *a);
    my_type *b = malloc(N * sizeof *a);
    my_type *c = malloc(N * sizeof *a);

    // Initialize a with some values.
    for (size_t i = 0; i < N; i++) {
        a[i] = i;
    }

    clock_t start, start_mcp, end, end_mcp;

    start = clock();
    for (size_t i = 0; i < N; i++) {
        b[i] = a[i];
    }
    end = clock();

    start_mcp = clock();
    memcpy(c, a, N * sizeof *a);
    end_mcp = clock();

    double total, total_mcp;
    total = (double)(end - start)/CLOCKS_PER_SEC;
    total_mcp = (double)(end_mcp - start_mcp)/CLOCKS_PER_SEC;

    printf("Time elapsed, for loop: %.2lf ms\n", total*1000);
    printf("Time elapsed, memcpy:   %.2lf ms\n", total_mcp*1000);
    printf("Speedup: %lf\n", total/total_mcp);

    return 0;
}
