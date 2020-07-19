#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void){
  int m, n ;
  double **A;
  clock_t start, timer_rowwise, timer_colwise;

  m = n = 10000;

  A = (double **)malloc(m * sizeof(double *));
  A[0] = (double *)malloc(m * n * sizeof(double));

  for (int i = 1; i<m; i ++){
    A[i] = &(A[0][n*i]);
  }

  start = clock();

  for(int i = 0; i < m;i ++){
    for (int j = 0; j < n; j++){
      A[i][j] = m * i + j;
    }
  }

  timer_rowwise = clock() - start;

  start = clock();

  for(int j = 0; j < m;j ++){
    for (int i = 0; i < n; i++){
      A[i][j] = m * i + j;
    }
  }

  timer_colwise = clock() - start;
  
  // Result
  printf("Time elapsed using i-j-loop: %lu millisec.\n", 1000*timer_rowwise/CLOCKS_PER_SEC);
  printf("Time elapsed using j-i-loop: %lu millisec.\n", 1000*timer_colwise/CLOCKS_PER_SEC);

  free(A[0]);
  free(A);

}
