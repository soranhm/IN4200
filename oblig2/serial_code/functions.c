#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <mpi.h>

void allocate_image(image *u, int m, int n){
  /*
  Function allocate image is supposed to allocate the 2D array image
  data inside u, when m and n are given as input.
  */
  u->m = m;
  u->n = n;
  // allocating m size 2D matrix
  u->image_data = (float**)malloc(m* sizeof(float*));
  // allocating m*n vectors
  int i;
  for (i = 0; i < m; i++) {
    u->image_data[i] = (float*)malloc(n * sizeof(float));
  }
}


void deallocate_image(image *u){
  /*
  The purpose of function deallocate image is to free the storage
  used by the 2D array image data inside u.
  */
  //deallocate all m*n vectors
  int i;
  for (i = 0; i < u->m; i++) {
    free(u->image_data[i]);
  }
  //deallocate m matrix
  free(u->image_data);
}


void convert_jpeg_to_image(const unsigned char* image_chars, image *u){
  /*
  Function convert jpeg to image is supposed to convert a
  1D array of unsigned char values into an image struct.
  */

  //Converting 1D unsigned char to 2D float
  int i,j;
  for (i = 0; i < u->m; i++) {
    for (j = 0; j < u->n; j++) {
      u->image_data[i][j] = (float)image_chars[i*u->n + j];
    }
  }
}


void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  /*
  Function convert image to jpeg does the conversion in
  the opposite direction.
  */

  // Converting 2D float back to 1D unsigned char
  int i ,j;
  for (i = 0; i < u->m; i++) {
    for (j = 0; j < u->n; j++) {
      image_chars[i*u->n + j] = (unsigned char)u->image_data[i][j];
    }
  }
}


void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters){
  /*
  he most important function that needs to be
  implemented is iso diffusion denoising, which is
  supposed to carry out iters iterations of isotropic
  diffusion on a noisy image object u. The denoised image
  is to be stored and returned in the u bar object.
  Note: After each iteration (except the last iteration),
  the two objects u and u bar should be swapped.
  */

  int n,m ;
  float** tmp;

  //  Boundaries
  //  for u[i][0], u[i][n-1], u[0][j] and u[m-1][j]

  //left and right boundary
  int i, j, k;
  for(i = 0; i < u->m ;i++){
    u_bar->image_data[i][0] = u->image_data[i][0];
    u_bar->image_data[i][u->n-1] = u->image_data[i][u->n-1];
  }

  //top and bottom boundary
  for(j = 0; j < u->n ;j++) {
    u_bar->image_data[0][j] = u->image_data[0][j];
    u_bar->image_data[u->m-1][j] = u->image_data[u->m-1][j];
  }
  printf("\033[1;32mSTARTING THE ITERATION...\033[0m\n");
  for(k = 0; k < iters; k++) {
    for (i = 1; i < u->m-1; i++) {
      for (j = 1; j < u->n-1; j++) {
        u_bar->image_data[i][j] = u->image_data[i][j] + kappa*(u->image_data[i-1][j]
                                  + u->image_data[i][j-1] - 4*u->image_data[i][j]
                                  + u->image_data[i][j+1] + u->image_data[i+1][j]);
      }
    }

    //updating
    tmp = u->image_data;
    u->image_data = u_bar->image_data;
    u_bar->image_data = tmp;
  }
  printf("\033[1;32mITERATION DONE...\033[0m\n");

}


void iso_diffusion_denoising_parallel(image* u, image* u_bar, float kappa, int iters){
  /*
  The new function iso diffusion denoising parallel needs to enhance its serial
  counterpart with necessary MPI communication calls.
  */
  float **temp, **u_, **u_b;
  int my_rank, num_procs;
  float *row_before,*row_after;
  int m,n;

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   // to get the rank
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // to get the num_procs

  // Rewriting to make the code nice
  m = u->m;
  n = u->n;
  u_ = u->image_data;
  u_b =u_bar->image_data;

  // Those rows are used for the calculation, to not get outside the matrix
  row_before = (float*)malloc(n * sizeof(float));
  row_after  = (float*)malloc(n * sizeof(float));

  printf("\033[1;3%dmSTARTING THE ITERATION...\033[0m\n",my_rank+1);
  int i, j ,k ;
  for (k = 0; k < iters; k++){
    if(num_procs==1){}  // if only one process runs, jumps to calculation as serial
    else if (my_rank == 0){ // if more then one processors, and my_rank = 0
      //MPI_Send(&u_[m-1][0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
      //MPI_Recv(&row_after[0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Sendrecv(&u_[m-1][0], n, MPI_FLOAT, my_rank+1, 0,
                   &row_after[0], n, MPI_FLOAT, my_rank+1, 0,
                   MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }
    else if (my_rank == num_procs - 1){ //Last rank
      //MPI_Recv(&row_before[0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //MPI_Send(&u_[0][0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
      MPI_Sendrecv(&u_[0][0], n, MPI_FLOAT, my_rank-1, 0,
                   &row_before[0], n, MPI_FLOAT, my_rank-1, 0,
                   MPI_COMM_WORLD, MPI_STATUS_IGNORE);



    }
    else{ //all in the middle
      //MPI_Recv(&row_before[0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //MPI_Send(&u_[0][0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
      //MPI_Send(&u_[m-1][0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
      //MPI_Recv(&row_after[0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      MPI_Sendrecv(&u_[0][0], n, MPI_FLOAT, my_rank-1, 0,
                   &row_before[0], n, MPI_FLOAT, my_rank-1, 0,
                   MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Sendrecv(&u_[m-1][0], n, MPI_FLOAT, my_rank+1, 0,
                  &row_after[0], n, MPI_FLOAT, my_rank+1, 0,
                  MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    // BOUNDARIES (ignoring left and right)
    for(j = 1; j < n - 1; j++){ //first row
      u_b[0][j] = u_[0][j] + kappa*(row_before[j]
                + u_[0][j-1] - 4*u_[0][j]
                + u_[0][j+1] + u_[1][j]);
    }

    for(j = 1; j < n - 1; j++){ //last row
      u_b[m-1][j] = u_[m-1][j] + kappa*(u_[m-2][j]
                  + u_[m-1][j-1] - 4*u_[m-1][j]
                  + u_[m-1][j+1] + row_after[j]);
    }


    //  REST
    for(i = 1; i < m - 1; i++){
      for(j = 1; j < n - 1; j++){
        u_b[i][j] = u_[i][j] + kappa*(u_[i-1][j]
                        + u_[i][j-1] - 4*u_[i][j]
                        + u_[i][j+1] + u_[i+1][j]);
      }
    }
    // updating values
    temp = u_;
    u_ = u_b;
    u_b = temp;
  }
  printf("\033[1;3%dmITERATION DONE...\033[0m\n",my_rank+1);
}
