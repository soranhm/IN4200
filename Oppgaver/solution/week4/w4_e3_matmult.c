// Simple matrix-matrix multiplication.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "week4.h"

int main(int argc, char const *argv[])
{

    // Testing multiplication.
    int n, m, p;
    n = 3;
    m = 3;
    p = 5;

    // Allocate matrices.
    double **A, **B, **C, **D;
    alloc2D(&A, n, m);
    alloc2D(&B, m, p);
    alloc2D(&C, n, p);
    alloc2D(&D, n, p);

    // Assign some values.
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            A[i][j] = i + j + 1;
        }
    }

    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            B[i][j] = i*j + 1;
        }
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < p; j++) {
            C[i][j] = 0;
            D[i][j] = 0;
        }
    }

    printf("A = \n");
    printmat(A, n, m);
    printf("B = \n");
    printmat(B, m, p);

    // Multiplication.
    matmult(A, B, C, n, m, p);
    matmultfast(A, B, D, n, m, p);

    printf("C = matmult(A, B)\n");
    printmat(C, n, p);

    printf("D = matmultfast(A, B)\n");
    printmat(D, n, p);

    // Free arrays.
    free2D(A);
    free2D(B);
    free2D(C);
    free2D(D);

    // Testing speedup.

    // Allocate matrices.

    n = 1000;
    m = 1000;
    p = 1000;

    alloc2D(&A, n, m);
    alloc2D(&B, m, p);
    alloc2D(&C, n, p);
    alloc2D(&D, n, p);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            A[i][j] = randfrom(-1, 1);
            B[i][j] = randfrom(-1, 1);
        }
    }

    clock_t start1 = clock();
    matmult(A, B, C, n, m, p);
    clock_t end1 = clock();

    clock_t start2 = clock();
    matmultfast(A, B, D, n, m, p);
    clock_t end2 = clock();

    double tot1 = (double) 1000*(end1 - start1)/CLOCKS_PER_SEC;
    double tot2 = (double) 1000*(end2 - start2)/CLOCKS_PER_SEC;

    printf("Total time matmult:     %.0lfms\n", tot1);
    printf("Total time matmultfast: %.0lfms\n", tot2);
    printf("Speedup: %.3lf\n", tot1/tot2);

    free2D(A);
    free2D(B);
    free2D(C);
    free2D(D);

    return 0;
}
