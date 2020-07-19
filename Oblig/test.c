#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


//globale variables i use
int n_nodes;
int n_edges;
double *values;
int *row_ptr;
int *col_idx;
int *w_place;
int w_dang;

void read_graph_from_file(char *filename){
  char *line = NULL;  //used for skip line
  size_t len = 0;     //used for skip line
  FILE *file;
  char err;

  file = fopen(filename, "r");//opening file

  if (file == NULL){
    perror("Error - could not open file...\n");
    exit(EXIT_FAILURE);
  }

  err = getline(&line, &len, file); //jumping a line
  err = getline(&line, &len, file); //jumping a line
  err = fscanf(file, "%*s %*s %d %*s %d", &n_nodes, &n_edges);  //saving nodes and edges
  err = getline(&line, &len, file); //jumping a line

  //making empty arrays for IDs
  int *F_nodeId = (int *)malloc(n_edges * sizeof(int));
  int *T_nodeId = (int *)malloc(n_edges * sizeof(int));

  // looping through file and saving IDs in array
  int s = 0;
  int a,b;
  while(s < n_edges) {
    err = getline(&line, &len, file);
    err = fscanf(file, "%d %d", &a, &b);
    if(a == b){
      n_edges--;
      continue;
    }
    F_nodeId[s] = a;
    T_nodeId[s] = b;
    s++;
  }
  fclose(file); // closing file
  printf("File read and closed...\n");

  row_ptr = (int*)malloc((n_nodes+1) * sizeof(int));
  col_idx = (int *)malloc(n_edges * sizeof(int));
  values = (double *)malloc(n_edges * sizeof(double));

  // array to add sum of same values
  int *total_F = (int *)malloc(n_nodes * sizeof(int));
  int *total_T = (int *)malloc(n_nodes * sizeof(int));

  //Sum of same values in F_nodeId and T_nodeId
  for (int l=0; l<n_nodes; l++) {
    total_F[l] = 0;
    total_T[l] = 0;
  }

  for(int k=0; k<n_edges; k++) {
    total_F[F_nodeId[k]]++;
    total_T[T_nodeId[k]]++;
  }

  printf("Finding the CRS arrays ...\n");
  //initialize  col_idx, values and row ptr

  row_ptr[0] = 0;
  int cnt = 0;
  for(int n=0; n < n_nodes; n++){
    cnt += total_T[n];
    row_ptr[n+1] = cnt;
  }


  int *temp = (int *)malloc(n_nodes * sizeof(int));
  for (int i = 0; i < n_nodes; i++){
    temp[i] = 0;
  }
  int count = 0;
  //making col_idx and values
  for (int j = 0; j < n_edges; j++) {
    count = row_ptr[T_nodeId[j]] + temp[T_nodeId[j]];
    col_idx[count] = F_nodeId[j];
    values[count] = (double)1/total_F[F_nodeId[j]];
    temp[T_nodeId[j]]++;
  }

  printf("Finding dangling points ...\n");
  w_place = (int *)malloc(n_nodes * sizeof(int));
  w_dang = 0;
  //finding dangling points
  for(int b=0; b < n_nodes; b++) {
    if (total_F[b] == 0) {
      w_place[w_dang] = b;
      w_dang++;
    }
  }
  printf("Total dangling points are: %d\n",w_dang );
  free(F_nodeId);
  free(T_nodeId);
  free(total_F);
  free(total_T);
  free(temp);
}

double* PageRank_iterations(double d, double eps) {
  double *xk = (double *)malloc(n_nodes * sizeof(double));
  double *xk_1 = (double *)malloc(n_nodes * sizeof(double));
  double sum_wk; // sum of xk[dangling] and k iteration
  double* tmp;  //to update xk_1
  double tamp;

  for(int i=0; i < n_nodes; i++){
    xk[i] = 1/(double)n_nodes;
    xk_1[i] = xk[i];
  }

  printf("Calculating xk ...\n");
  bool array_ok = false;
  int k = 0;
  while (!array_ok) {
    array_ok = true;
    k++;
    sum_wk = 0;
    // calculating W
    for(int i=0; i<w_dang;i++){
      sum_wk += xk_1[w_place[i]];
    }

    tamp = (1 - d + (d * sum_wk))/((double)n_nodes);
    int j;
    //calculating x
    for (int i = 0; i < n_nodes; i++) {
      xk[i] = tamp; // calculating outside for loop since const
      for (j = row_ptr[i]; j < row_ptr[i+1]; j++) {
        xk[i] += d * (values[j] * xk_1[col_idx[j]]);
      }
      // Stopping crirerion
      if (fabs(xk[i]-xk_1[i])>eps){
        array_ok = false;
      }
    }
    //updating values
    tmp = xk_1;
    xk_1 = xk;
    xk = tmp;
  }
  printf("\nTotal  iterations : %d\n",k);
  free(xk_1);
  free(row_ptr);
  free(col_idx);
  free(values);
  free(w_place);
  free(tmp);
  return xk;
}

int top_n_webpages(double* xk,int top_n) {
  int *top_n_index = (int*)malloc(sizeof(int) * top_n);
  double max = 0, prevmax = 0;
  int idx;

  //finding first max value
  for (size_t i = 0; i < n_nodes; i++) {
    if(xk[i] > max) {
      max = xk[i];
      idx = i;
    }
  }
  top_n_index[0] = idx;
  prevmax = max;

  for (size_t i = 1; i < top_n; i++) {
    max = 0;
    for (size_t j = 0; j < n_nodes; j++) {
      if(xk[j] > max && xk[j] < prevmax) {
        max = xk[j];
        idx = j;
      }
    }
    top_n_index[i] = idx;
    prevmax = max;
  }
/*
  printf("    PAGE    | MAX VALUE\n" );
  for (int i = 0; i < top_n; i++) {
    printf(" %7d  | %f\n", top_n_index[i]+1, xk[top_n_index[i]]);
  }*/
  free(top_n_index);
  return 0;
}
