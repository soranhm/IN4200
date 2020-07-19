#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

double compute_sum_v1(int rank, int numprocs, int master, double mysum);
double compute_sum_v2(int rank, int numprocs, int master, double mysum);
double compute_sum_v3(int rank, int numprocs, int master, double mysum);

#define N 10000

int main(int argc, char **argv)
{

    int myrank, numprocs;
    double mysum, totalsum[3];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);


    /* generate random sum */
    srand48(time(NULL) + myrank);
    mysum = 0;
    for (int i = 0; i < N; i ++)
    {
        mysum += drand48();
    }

    totalsum[0] = compute_sum_v1(myrank, numprocs, 0, mysum);
    totalsum[1] = compute_sum_v2(myrank, numprocs, 0, mysum);
    totalsum[2] = compute_sum_v3(myrank, numprocs, 0, mysum);

    if (myrank == 0)
    {
        printf("Sum computed via sends: %lf\n", totalsum[0]);
        printf("Sum computed via gather: %lf\n", totalsum[1]);
        printf("Sum computed via reduce: %lf\n", totalsum[2]);
    }

    MPI_Finalize();
    return 0;
}

/* Solution 1: sum collected via sends by master. */
double compute_sum_v1(int rank, int numprocs, int master, double mysum)
{
    double totalsum, recvsum;
    MPI_Status status;

    if (rank == master)
    {
        totalsum = mysum;
        for (int p = 0; p < numprocs; p ++)
        {
            if (p == rank)
            {
                continue;
            }
            MPI_Recv(&recvsum, 1, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, &status);
            totalsum += recvsum;
        }
    }
    else
    {
        MPI_Send(&mysum, 1, MPI_DOUBLE, master, 0, MPI_COMM_WORLD);
    }

    return totalsum;
}

/* Version 2: sums collected via gather by master... */
double compute_sum_v2(int rank, int numprocs, int master, double mysum)
{
    double totalsum, *everyones_sums;
    everyones_sums = malloc(numprocs * sizeof *everyones_sums);

    MPI_Gather(&mysum, 1, MPI_DOUBLE, everyones_sums, 1, MPI_DOUBLE, master, MPI_COMM_WORLD);

    totalsum = 0;
    if (rank == master)
    {
        for (int i = 0; i < numprocs; i ++)
        {
            totalsum += everyones_sums[i];
        }
    }

    return totalsum;
}

/* Version 3: sums collected via reduce like we shoulda done in the first place. */
double compute_sum_v3(int rank, int numprocs, int master, double mysum)
{
    double totalsum;

    MPI_Reduce(&mysum,
               &totalsum,
               1,
               MPI_DOUBLE,
               MPI_SUM,
               master,
               MPI_COMM_WORLD);

    return totalsum;
}
