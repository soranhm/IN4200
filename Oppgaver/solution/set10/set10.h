#ifndef SET10_H
#define SET10_H

// Header with a matvec multiplication implementation used to check our MPI
// implementation.

#define idx(i,j) (i*N + j)


int matvec_mult(double* A, double *x, double *y, int N){
    for (size_t i = 0; i < N; i++) {
        y[i] = 0;
        for (size_t j = 0; j < N; j++) {
            y[i] += A[idx(i,j)]*x[j];
        }
    }
    return 0;
}

int printvec(double* y, int N){
    printf("[%.3lf", y[0]);
    for (size_t i = 1; i < N; i++) {
        printf(", %.3lf", y[i]);
    }
    printf("]\n");
    return 0;
}

int printmat(double* A, int N, int M){
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            printf("%7.2lf ", A[i*M + j]);
        }
        printf("\n");
    }
    return 0;
}

double sum_err_sqr(double* y, double* y2, int N){
    double error = 0;
    for (size_t i = 0; i < N; i++) {
        error += (y[i] - y2[i])*(y[i] - y2[i]);
    }
    return error;
}

#endif
