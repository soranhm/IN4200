#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "PE_functions_15350.c"


int main(int narg, char *argv[]) {
  char* filename;
  double d,eps;
  int n;
  double *xk;

  filename = argv[1];
  d = atof(argv[2]);
  eps = atof(argv[3]);
  n = atoi(argv[4]);

  if (narg < 2) {
    printf("Filename required.\n");
    printf("Running example: ./a.out filename d eps n\n");
    exit(0);
  }

  printf("Reading file...\n");
  double begin1 = omp_get_wtime();
  read_graph_from_file(filename);
  double read_time1 = omp_get_wtime();

  printf("Iterating true x_k...\n");
  double begin2 = omp_get_wtime();
  xk = PageRank_iterations(d,eps);
  double read_time2 = omp_get_wtime();


  printf("Finding top %d values...\n",n);
  double begin3 = omp_get_wtime();
	top_n_webpages(xk,n);
  double read_time3 = omp_get_wtime();

  printf("Reading graph from file: took %f seconds\n", ((double)(read_time1 - begin1)));
  printf("PageRank iterations:     took %f seconds\n", ((double)(read_time2 - begin2)));
  printf("Top n webpages:          took %f seconds\n", ((double)(read_time3 - begin3)));
  double total_time = (double)(read_time3 - begin1);
  printf("ALL DONE!!,      TOTAL TIME = %f seconds\n",total_time);
  return 0;
}
