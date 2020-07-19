// Solution to week 8 ex. 3 a-d.

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "oddeven.h"

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Usage: ./my_prog N\n");
        return 1;
    }
    int N = atoi(argv[1]);

    int *a = malloc(N * sizeof *a);
    int *b = malloc(N * sizeof *b);
    int *c = malloc(N * sizeof *c);
    int *d = malloc(N * sizeof *d);
    for (size_t i = 0; i < N; i++) {
        a[i] = N-i;
        b[i] = N-i;
        c[i] = N-i;
        d[i] = N-i;
    }

    printf("Before sort:\n");
    printvec(a, N);

    oddeven(a, N);
    printf("After oddeven, %d iterations:\n", N);
    printvec(a, N);


    oddeven_anylenght(b, N);
    printf("After oddeven_anylenght, %d iterations:\n", N);
    printvec(b, N);

    int iter;
    iter = oddeven_stop(c, N);
    printf("After oddeven_stop, %d iterations:\n", iter);
    printvec(c, N);

    oddeven_omp(d, N);
    printf("After oddeven_omp:\n");
    printvec(d, N);


    return 0;
}
