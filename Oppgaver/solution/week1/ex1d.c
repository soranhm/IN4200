#include <stdio.h>
#include <stdlib.h>

/*
This is an understandably difficult exercise for many (including myself, even today).
I recommend starting with ex1c, which demonstrates a contigious 2d array. The
underlying idea is the same, but it's very easy to mess up.

I have added a few print statements so you can verify that the resulting array
looks as it should.
*/
int main(void) {

    int Nx, Ny, Nz;

    int ***A;

    Nx = Ny = Nz = 3;

    A = (int ***)malloc(Nz * sizeof(int **));
    A[0] = (int **)malloc(Nz * Ny * sizeof(int *));
    A[0][0] = (int *)malloc(Nz * Ny * Nx * sizeof(int));

    // First layer of pointers.
    // The 3d array contains Nz "slices," each of which has Ny columns.
    for (int i = 1; i < Nz; i ++) {
        A[i] = &A[0][Ny * i];
    }

    // Second layer of pointers.
    // The 3d array contains Ny * Nz rows, each of which has Nx cells.
    for (int j = 1; j < Nz * Ny; j ++) {
        A[0][j] = &A[0][0][j * Nx];
    }

    // Assign some values.
    for (int i = 0; i < Nz; i ++) {
        for (int j = 0; j < Ny; j ++) {
            for (int k = 0; k < Nx; k ++) {
                A[i][j][k] = Nx * Ny * i + Ny * j + k;
                printf("(%d %d %d): %d ", i, j, k, A[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    // Last step
    free(A[0][0]);
    free(A[0]);
    free(A);

}