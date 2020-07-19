#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "test.c"


int main(int narg, char *argv[]) {
  char* filename;
  double d,eps;
  int n;
  double *xk;

  filename = "web-NotreDame.txt";
  d = 0.85;
  eps = 1e-10;
  n = 10000;
/*
  if (narg < 2) {
    printf("Filename required.\n");
    printf("Running example: ./a.out filename d eps n\n");
    exit(0);
  }*/
  printf("Reading file...\n");
  clock_t begin = clock();
  read_graph_from_file(filename);
  clock_t end = clock();

  printf("Iterating true x_k...\n");
  clock_t begin2 = clock();
  xk = PageRank_iterations(d,eps);
  clock_t end2 = clock();


  printf("Finding top %d values...\n",n);
  printf("Iterating true x_k...\n");
  clock_t begin3 = clock();
  top_n_webpages(xk,n);
  clock_t end3 = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  double time_spent2 = (double)(end2 - begin2) / CLOCKS_PER_SEC;
  double time_spent3 = (double)(end3 - begin3) / CLOCKS_PER_SEC;
  printf("WITHOUT OPENMP\n");
  printf("Reading graph from file: took %f seconds\n", time_spent);
  printf("PageRank iterations:     took %f seconds\n", time_spent2);
  printf("Top n webpages:          took %f seconds\n", time_spent3);
  double total_time = (double)(end3 - begin)/ CLOCKS_PER_SEC;
  printf("ALL DONE!!,      TOTAL TIME = %f seconds\n",total_time);
  return 0;
}
