// MPI implementation of a distributed matrix-vector multiplication.
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "set10.h"

// Used to index local copies of the data in matrix A ex. 4.
#define idx2(i,j) (i*n_cols[myrank] + j)

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
    int *n_cols = malloc(numprocs*sizeof *n_cols);
    int *displs = malloc(numprocs*sizeof *displs);

    int cols = N/numprocs;
    int remainder = N%numprocs;

    // Last remainder processes gets an extra row.
    displs[0] = 0;
    for (int rank = 0; rank < numprocs-1; rank++) {
        n_cols[rank] = cols + ((rank >= (numprocs - remainder)) ? 1:0);
        displs[rank+1] = displs[rank] + n_cols[rank];
    }
    n_cols[numprocs-1] = cols + ((numprocs-1) >= (numprocs - remainder) ? 1:0);


    // Allocate local buffers.
    double *A;
    double *x;
    if (myrank == 0) {
        A = malloc(N*N * sizeof *A);
        x = malloc(N * sizeof *x);
        // Initialize to some values:
        for (size_t i = 0; i < N; i++) {
            x[i] = i%4 + 1;
            for (size_t j = 0; j < N; j++) {
                A[idx(i,j)] = 0.01*i + 0.01*j;
            }
        }
        if (N <= 20) {
            printf("A=\n");
            printmat(A, N, N);
            printf("x=\n");
            printvec(x, N);
            printf("\n");
        }
    }
    double *myA = malloc(N*n_cols[myrank] * sizeof *myA);
    double *my_x = malloc(n_cols[myrank] * sizeof *my_x);
    MPI_Barrier(MPI_COMM_WORLD);
    // Define types.
    MPI_Datatype col_temp, col_vec, recv_temp, recv_col;

    // This is the type used to send the data.
    MPI_Type_vector(N, 1, N, MPI_DOUBLE, &col_temp);
    // This line is necesarry, but a bit confusing. See the README.
    MPI_Type_create_resized(col_temp, 0, sizeof(double), &col_vec);
    MPI_Type_commit(&col_vec);

    // This type is used to recive. Only difference is the stride.
    MPI_Type_vector(N, 1, n_cols[myrank], MPI_DOUBLE, &recv_temp);
    MPI_Type_create_resized(recv_temp, 0, sizeof(double), &recv_col);
    MPI_Type_commit(&recv_col);


    // Scatter A and x.
    MPI_Scatterv(A,                 // sendbuf, matters only for root process.
                 n_cols,
                 displs,
                 col_vec,           // The new type used to send.
                 myA,
                 n_cols[myrank],
                 recv_col,          // New type used to receive.
                 0,
                 MPI_COMM_WORLD);

    MPI_Scatterv(x,
                 n_cols,
                 displs,
                 MPI_DOUBLE,
                 my_x,
                 n_cols[myrank],
                 MPI_DOUBLE,
                 0,
                 MPI_COMM_WORLD);



    // Actual matrix-vector multiplication
    double *my_y = malloc(N * sizeof *my_y);

    for (size_t i = 0; i < N; i++) {
        my_y[i] = 0;
        for (size_t j = 0; j < n_cols[myrank]; j++) {
            my_y[i] += myA[idx2(i,j)] * my_x[j];
        }
    }

    double *y;
    if (myrank==0) {
        y = malloc(N * sizeof *y);
    }
    MPI_Reduce(my_y, // Send buffer.
               y, // Receive buffer.
               N,
               MPI_DOUBLE,
               MPI_SUM,
               0, // Root, the result ends up here.
               MPI_COMM_WORLD);

    // Print the results and compare.
    if (myrank == 0) {
        double *y2 = malloc(N * sizeof *y2);
        matvec_mult(A, x, y2, N);

        if (N <= 20) {
            printf("MPI results:\n");
            printvec(y, N);

            printf("Serial results:\n");
            printvec(y2, N);
        }

        double error = sum_err_sqr(y, y2, N);
        printf("Sum error squared:\n");
        printf("%lf\n", error);

        free(y2);
        free(A);
        free(y);
        free(x);
    }

    free(myA);
    free(my_x);
    free(n_cols);
    free(displs);

    MPI_Finalize();

    return 0;
}
