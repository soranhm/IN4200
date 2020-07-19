#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    char msg[256];
    sprintf(msg, "Hello World from process %d", my_rank);
    int msglen = strlen(msg) + 1;  // The +1 includes the terminating char.
    print(msglen);
    // Communicate the messages and print them: Note reuse of msg buffer.
    MPI_Status msg_status;
    if (my_rank == 0) {
        printf("%s\n", msg);
        for (size_t i = 1; i < num_procs; i++) {
            MPI_Probe(i, 0, MPI_COMM_WORLD, &msg_status);
            MPI_Get_count(&msg_status, MPI_CHAR, &msglen);

            MPI_Recv(msg, msglen, MPI_CHAR, i, 0, MPI_COMM_WORLD, &msg_status);
            printf("%s\n", msg);
        }
    }
    else {
        MPI_Send(msg, msglen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
