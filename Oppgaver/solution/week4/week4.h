#ifndef WEEK4_H
#define WEEK4_H

#include <stdlib.h> // rand, malloc and free.
#include <stdio.h>  // printf

// One of the ways to document C and C++ functions are to create a block comment
// before the function. There are several styleguides out there you can follow.
// If you don't have any preference i suggest using the google style guide.
// https://google.github.io/styleguide/cppguide.html#Function_Comments


// Return random double in range [min, max].
//
// Source : https://ubuntuforums.org/showthread.php?t=1717717&p=10618266#post10618266
double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}


// Compute matrix multplication C = A*B
//
// Parameters
// ----------
// A, B : matrices of doubles. n*m and m*p.

// Return Parameters
// -----------------
// C : matrix of doubles, n*p
//      Assumed to be zero initialized.
int matmult(double **A, double **B, double **C, int n, int m, int p)
{
    double sum;
    for (size_t i = 0; i < n; i++) { // First dim in C.
        for (size_t j = 0; j < p; j++) { // Second dim in C.
            for (size_t k = 0; k < m; k++) { // Inner dim in A and B.
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
    return 0;
}

// Assumes zero initialized C.
int matmultfast(double **A, double **B, double **C, int n, int m, int p)
{
    int rest = p%4;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < p-rest; j+=4) { // Stride by 4.
            for (size_t k = 0; k < m; k++) { // Inner dim in A and B.
                C[i][j]   += A[i][k]*B[k][j];
                C[i][j+1] += A[i][k]*B[k][j+1];
                C[i][j+2] += A[i][k]*B[k][j+2];
                C[i][j+3] += A[i][k]*B[k][j+3];
            }
        // Rest loop.
        for (size_t j = p-rest; j < p; j++){
            for (size_t k = 0; k < m; k++){
                C[i][j]   += A[i][k]*B[k][j];
            }
        }
        }
    }


    return 0;
}



// This is just the same allocation you saw in week 1 and 2, but now done in a
// funnction. To be able to return the value of the pointers we need to send in
// the address of the pointer to our matrix, or a tripple pointer if you will.
// We then dereference this pointer using (*A), the parentheses is there to make
// sure that the pointer is dereferenced before any other operation.
// Notice that we return 1 if malloc fails. Using a return value greater than 0
// usually means somehting went wrong.
int alloc2D(double ***A, int n, int m)
{
    *A = malloc(n * sizeof *A);
    (*A)[0] = malloc(n*m * sizeof (*A)[0]);
    if (!(*A)[0] || !*A){
        // Allocation failed.
        printf("Allocation failed\n");
        return 1;
    }

    for (size_t i = 1; i < n; i++) {
        (*A)[i] = &((*A)[0][i*m]);
    }
    return 0;
}


// Free pointers allocated in alloc2D.
int free2D(double **A)
{
    free(A[0]);
    free(A);
    return 0;
}

// Print matrix values.
int printmat(double **A, int n, int m)
{
    for (size_t i = 0; i < n; ++i){
        printf("| ");
        for (size_t j = 0; j < m; ++j){
            printf("%4.0lf ", A[i][j]);
        }
        printf("|\n");
    }
}

#endif
