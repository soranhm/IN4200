#include <stdlib.h>
#include <stdio.h>
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

    // Assign some values
    
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            A[idx(i,j)] = i + j;
        }
    }

    for (size_t j = 0; j < n; j++) {
        y[j] = j;
    }

    dense_mat_vec(m, n, x, A, y);

    // Print result
    if (m <= 10) {
        printf("x=\n[");
        for (size_t i = 0; i < m-1; i++) {
            printf("%.0lf, ", x[i]);
        }
        printf("%.0lf]\n", x[m-1]);
    }

    free(A);
    free(y);
    free(x);

    return 0;
}
