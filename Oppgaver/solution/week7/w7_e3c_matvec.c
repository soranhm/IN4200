#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "matvec.h"

// Indexing function.
#define idx(i,j) (i*n + j)

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Must provide m and n.\n");
        return 1;
    }
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    double *A = malloc(m*n*sizeof *A); // m*n matrix
    double *y = malloc(n * sizeof *y); // n*1 vector
    double *x = malloc(m * sizeof *x); // m*1 vector

    // Assign some values in parallel
    #pragma omp parallel for
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            A[idx(i,j)] = i + j;
        }
    }

    #pragma omp parallel for
    for (size_t j = 0; j < n; j++) {
        y[j] = j;
    }

    double start_p = omp_get_wtime();
    #pragma omp parallel
    {
      int thread_id = omp_get_thread_num();
      int num_threads = omp_get_num_threads();

      // Using integer division to divide up the rows means that the last
      // remainder = m%num_threads, gets an extra row.
      int start_m = (thread_id*m)/num_threads;
      int stop_m = ((thread_id+1)*m)/num_threads;

      // calls the serial dense_mat_vec function
      dense_mat_vec(stop_m-start_m, n, &x[start_m], &A[start_m*n], y);
    }
    double end_p = omp_get_wtime();
    double tot_p = end_p - start_p;

    // Print result
    if (m <= 20) {
        printf("x=\n[");
        for (size_t i = 0; i < m-1; i++) {
            printf("%.0lf, ", x[i]);
        }
        printf("%.0lf]\n", x[m-1]);
    }

    double start_s = omp_get_wtime();
    dense_mat_vec(m, n, x, A, y);
    double end_s = omp_get_wtime();
    double tot_s = end_s - start_s;

    // Print result for comparison
    if (m <= 20) {
        printf("x=\n[");
        for (size_t i = 0; i < m-1; i++) {
            printf("%.0lf, ", x[i]);
        }
        printf("%.0lf]\n", x[m-1]);
    }

    printf("Time serial:   %lf\n", tot_s);
    printf("Time parallel: %lf\n", tot_p);
    printf("Speedup: %lf\n", tot_s/tot_p);

    free(A);
    free(y);
    free(x);

    return 0;
}
