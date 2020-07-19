#ifndef MATVEC_H
#define MATVEC_H

#include <stdlib.h> // malloc and free.
#include <stdio.h>  // printf
#include <omp.h>

// The function as give in the exercise.
void dense_mat_vec(int m, int n, double *x, double *A, double *y)
{
    int i, j;
    for (i=0; i<m; i++){
        double tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j] * y[j];
        x[i] = tmp;
    }
}

void dense_mat_vec_omp(int m, int n, double *x, double *A, double *y)
{
    int i, j;
    #ifdef _OPENMP
    #pragma omp parallel for private(j) // So simple!
    #endif
    for (i=0; i<m; i++){
        double tmp = 0.;
        for (j=0; j<n; j++)
            tmp += A[i*n+j] * y[j];
        x[i] = tmp;
    }
}


#endif
