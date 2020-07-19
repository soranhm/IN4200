// MPI implementation of a distributed matrix-vector multiplication.
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "set10.h"

int main(int argc, char *argv[]) {
    int myrank, numprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // cmd line arguments in MPI programs depend on the specific implementation
    // you are using. The safest way to make sure all processes gets the
    // arguments is to broadcast them.
    int N;
    if (myrank == 0) {
        if (argc>1) {
            N = atoi(argv[1]);
        } else {
            N = numprocs*3 + 1;
        }
        printf("N = %d\n", N);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate displacements and number of rows for each process.
    int *n_rows = malloc(numprocs*sizeof *n_rows);

    // Used when scattering A.
    int *sendcounts = malloc(numprocs*sizeof *sendcounts);
    int *Sdispls = malloc(numprocs*sizeof *Sdispls);

    // Used when gathering y.
    int *Gdispls = malloc(numprocs*sizeof *Gdispls);


    int rows = N/numprocs;
    int remainder = N%numprocs;
    Sdispls[0] = 0;
    Gdispls[0] = 0;

    // Last remainder processes gets an extra row.
    for (int rank = 0; rank < numprocs-1; rank++) {
        n_rows[rank] = rows + ((rank >= (numprocs - remainder)) ? 1:0);
        sendcounts[rank] = n_rows[rank]*N;
        Sdispls[rank+1] = Sdispls[rank] + sendcounts[rank];
        Gdispls[rank+1] = Gdispls[rank] + n_rows[rank];
    }
    n_rows[numprocs-1] = rows + ((numprocs-1) >= (numprocs - remainder) ? 1:0);

    sendcounts[numprocs-1] = n_rows[numprocs-1]*N;

    // Allocate local buffers.
    double *A;
    double *x = malloc(N * sizeof *x);
    if (myrank == 0) {
        A = malloc(N*N * sizeof *A);

        // Initialize to some values:
        for (size_t i = 0; i < N; i++) {
            x[i] = i%4;
            for (size_t j = 0; j < N; j++) {
                A[idx(i,j)] = 0.01*i + 0.01*j;
            }
        }
    } else {
        A = malloc(N*n_rows[myrank] * sizeof *A);
    }

    // Broadcast x and scatter A.
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(A,                 // Sendbuff, matters only for root process.
                 sendcounts,
                 Sdispls,
                 MPI_DOUBLE,
                 A,                 // Recieve buff is the same as sendbuf here.
                 N*n_rows[myrank],
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);


    // Actual matrix-vector multiplication
    double *y;
    if (myrank == 0) {
        y = malloc(N * sizeof *y); // We are going to gather in to this buffer.
    } else {
        y = malloc(n_rows[myrank] * sizeof *y);
    }

    for (size_t i = 0; i < n_rows[myrank]; i++) {
        y[i] = 0;
        for (size_t j = 0; j < N; j++) {
            y[i] += A[idx(i,j)] * x[j];
        }
    }

    // Gather the results
    MPI_Gatherv(y,
                n_rows[myrank],
                MPI_DOUBLE,
                y,              // Matters only at root,
                n_rows,
                Gdispls,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

    // Print the results and compare.
    if (myrank == 0) {
        double *y2 = malloc(N * sizeof *y2);
        matvec_mult(A, x, y2, N);

        if (N < 21) {
            printf("MPI results:\n");
            printvec(y, N);

            printf("Serial results:\n");
            printvec(y2, N);
        }

        double error = sum_err_sqr(y, y2, N);
        printf("Sum error squared:\n");
        printf("%lf\n", error);

        free(y2);
    }

    free(A);
    free(x);
    free(y);
    free(n_rows);
    free(Sdispls);
    free(Gdispls);
    free(sendcounts);

    MPI_Finalize();

    return 0;
}
