// Unrolling multiplication of a lower triangular matrix with a vector.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "week5.h"

int triangularmult(double **A, double *B, int N, double *y);
int triangmultunroll(double **A, double *B, int N, double *y);

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Usage: ./my_prog.exe N\n");
        return 1;
    }

    int N = 7;

    double *y = calloc(N, sizeof *y); // Sets memory to zero.
    double *z = calloc(N, sizeof *z);
    double *B = malloc(N * sizeof *B);

    double **A;
    alloc2D(&A, N, N);

    // Initialize some values.
    for (size_t i = 0; i < N; i++) {
        B[i] = i+1;
        for (size_t j = 0; j < N; j++) {
            A[i][j] = i+j;
        }
    }

    printf("Test of code transformation:\n");
    printf("A = \n");
    printmat(A, N, N);
    printf("B = \n");
    printvec(B, N);

    triangularmult(A, B, N, y);
    triangmultunroll(A, B, N, z);

    printf("Regular version result:\n");
    printvec(y, N);
    printf("Unrolled version result:\n");
    printvec(z, N);


    free(y);
    free(z);

    free2D(A);

    // Speed test:
    N = atoi(argv[1]);

    y = calloc(N, sizeof *y);
    z = calloc(N, sizeof *z);
    B = realloc(B, N * sizeof *B);

    alloc2D(&A, N, N);

    printf("\nTesting speedup with N = %d\n", N);

    clock_t start1 = clock();
    triangularmult(A, B, N, y);
    clock_t end1 = clock();
    double tot1 = (double)(end1 - start1)/CLOCKS_PER_SEC;

    clock_t start2 = clock();
    triangmultunroll(A, B, N, z);
    clock_t end2 = clock();
    double tot2 = (double)(end2 - start2)/CLOCKS_PER_SEC;

    printf("Speedup = %.3lf\n", tot1/tot2);

    // Freeing memory right before termination of a program is unnecessary,
    // The memory will be erleased anyway.
    free(y);
    free(z);
    free(B);
    free2D(A);

    return 0;
}


int triangularmult(double **A, double *B, int N, double *y)
{
    for (size_t j = 0; j < N; j++) {
        for (size_t i = 0; i <= j; i++) {
            y[j] += A[j][i]*B[i];
        }
    }
    return 0;
}


int triangmultunroll(double **A, double *B, int N, double *y)
{
    int remainder = N%4; // unroll four-way.

    // If N is not divisible by N we need to do some iterations the slow way.
    for (size_t j = 0; j < remainder; j++) {
        for (size_t i = 0; i <=  j; i++) {
            y[j] += A[j][i]*B[i];
        }
    }
    // Unrolled part.
    for (size_t j = remainder; j < N; j+=4) { // Stride of 4.
        for (size_t i = 0; i <= j; i++) {
            y[j]   += A[j][i] * B[i];
            y[j+1] += A[j+1][i]*B[i];
            y[j+2] += A[j+2][i]*B[i];
            y[j+3] += A[j+3][i]*B[i];
        }
        // These values are not included in the unrolled loop.
        y[j+1] += A[j+1][j+1]*B[j+1];
        y[j+2] += A[j+2][j+1]*B[j+1];
        y[j+2] += A[j+2][j+2]*B[j+2];
        y[j+3] += A[j+3][j+1]*B[j+1];
        y[j+3] += A[j+3][j+2]*B[j+2];
        y[j+3] += A[j+3][j+3]*B[j+3];
    }
    return 0;
}
