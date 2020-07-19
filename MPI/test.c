#include "mpi.h"
#include <stdio.h>
int main( int argc, char *argv[] )
{
  int errs = 0, provided, claimed;
  MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided );
  MPI_Query_thread( &claimed );
  if (claimed != provided) {
    errs++;
    printf("Query thread gave thread level %d but Init_thread gave %d\n",
           claimed, provided );fflush(stdout);
}
  MPI_Finalize();
  return errs;
}
