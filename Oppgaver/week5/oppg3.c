// Optimizing

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "week5.h"

void foo (int N, double **mat, double **s, int *v);
void foofast (int N, double **mat, double **s, int *v);
void foofaster (int N, double **mat, double **s, int *v);

int main(int argc, char const *argv[])
{

    double **mat1, **mat2, **s;
    int N, *v;

    if (argc < 2) {
        N = 2500;
    } else {
        N = atoi(argv[1]);
    }
    v = malloc(N * sizeof *v);

    alloc2D(&mat1, N, N);
    alloc2D(&mat2, N, N);
    alloc2D(&s, N, N);

    // Initialize some values.
    for (size_t i = 0; i < N; i++) {
        v[i] = i - N/2;
        for (size_t j = 0; j < N; j++) {
            s[i][j] = j + 1;
        }
    }

    foo(N, mat1, s, v);

    clock_t start1 = clock();
    foo(N, mat1, s, v);
    clock_t end1 = clock();

    clock_t start2 = clock();
    foofast(N, mat2, s, v);
    clock_t end2 = clock();

    double err_sum = 0;
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            err_sum = fabs(mat1[i][j] - mat2[i][j]);
        }
    }

    if (err_sum > 1e-7) {
        printf("Error of foofast larger than tolerance:\n");
        printf("err_sum = %lf\n", err_sum);
    }

    clock_t start3 = clock();
    foofaster(N, mat2, s, v);
    clock_t end3 = clock();

    err_sum = 0;
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            err_sum = fabs(mat1[i][j] - mat2[i][j]);
        }
    }

    if (err_sum > 1e-7) {
        printf("Error of foofaster larger than tolerance:\n");
        printf("err_sum = %lf\n", err_sum);
    }

    double tot1 = (double) (end1 - start1)/CLOCKS_PER_SEC;
    double tot2 = (double) (end2 - start2)/CLOCKS_PER_SEC;
    double tot3 = (double) (end3 - start3)/CLOCKS_PER_SEC;

    printf("Timing with N = %d\n", N);
    printf("Time foo:       %es\n", tot1);
    printf("Time foofast:   %es\n", tot2);
    printf("Time foofaster: %es\n", tot3);
    printf("Speedup foo vs foofast: %lf\n", tot1/tot2);
    printf("Speedup foo vs foofaster: %lf\n", tot1/tot3);

    free2D(mat1);
    free2D(mat2);
    free2D(s);

    return 0;
}

void foo (int N, double **mat, double **s, int *v)
{
    int i,j;
    double val;
    for (j=0; j<N; j++) {
        for (i=0; i<N; i++) {
            val = 1.0*(v[j]%256);
            mat[i][j] = s[i][j]*(sin(val)*sin(val)-cos(val)*cos(val));
        }
    }
}

void foofast (int N, double **mat, double **s, int *v)
{
    int i,j;
    double val;
    for (j=0; j<N; j++) {
        val = -cos(2.0*(v[j]%256));
        for (i=0; i<N; i++) {
            mat[i][j] = s[i][j]*val;
        }
    }
}

void foofaster (int N, double **mat, double **s, int *v)
{
    double *val = malloc(256 * sizeof *val);
    int *vtab = malloc(N * sizeof *vtab);
    // Table of possible values. This should really be constructed once,
    // outside the scope of this function.
    for (int i = 0; i < 256; i++) {
        val[i] = -cos(2.0*i);
    }
    // Table matching values of v to values in val.
    for (size_t i = 0; i < N; i++) {
        vtab[i] = abs(v[i]%256);
    }
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            mat[j][i] = s[j][i]*val[vtab[i]];
        }
    }
    free(val);
    free(vtab);
}
