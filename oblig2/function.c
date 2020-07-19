#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <mpi.h>

#include "function.h"
// #include "../simple-jpeg/import_export_jpeg.h"

void iso_diffusion_denoising_parallel(image *u, image *u_bar, float kappa, int iters){
  // int m, n, prev, next, my_rank, num_procs, last_row_loc;
  float *prev_row, *next_row;
  // float **u_, **u_b, **temp;
  image temp;
  int m, n, my_rank, num_procs, prev, next;

  m = u->m;
  n = u->n;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);   // to get the rank
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // to get the num_procs
  //
  // if (my_rank == 0){
  //   prev = my_rank;
  //   next = my_rank + 1;
  // }
  // else if (my_rank == (num_procs - 1)){
  //   prev = my_rank - 1;
  //   next = my_rank;
  // }
  // else{
  //   prev = my_rank - 1;
  //   next = my_rank + 1;
  // }
  //
  prev_row = (float*)malloc(n * sizeof(float));
  next_row = (float*)malloc(n * sizeof(float));

  // last_row_loc = (m - 1)*n;
  for (int iter = 0; iter < iters; iter++){
    if (my_rank == 0){
      MPI_Send(&(*u).image_data[m-1][0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
      MPI_Recv(&next_row[0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int j = 1; j < n - 1; j++){
        (*u_bar).image_data[m-1][j] = (*u).image_data[m-1][j] + kappa*((*u).image_data[m-2][j] + (*u).image_data[m-1][j-1] - 4*(*u).image_data[m-1][j] + (*u).image_data[m-1][j+1] + next_row[j]);
      }
    }
    else if (my_rank == num_procs - 1){
      MPI_Recv(&prev_row[0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(&(*u).image_data[0][0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
      for(int j = 1; j < n - 1; j++){
        (*u_bar).image_data[0][j] = (*u).image_data[0][j] + kappa*(prev_row[j] + (*u).image_data[0][j-1] - 4*(*u).image_data[0][j] + (*u).image_data[0][j+1] + (*u).image_data[1][j]);
      }
    }
    else{
      MPI_Recv(&prev_row[0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(&(*u).image_data[0][0], n, MPI_FLOAT, my_rank-1, 0, MPI_COMM_WORLD);
      MPI_Send(&(*u).image_data[m-1][0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD);
      MPI_Recv(&next_row[0], n, MPI_FLOAT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int j = 1; j < n - 1; j++){
        (*u_bar).image_data[0][j] = (*u).image_data[0][j] + kappa*(prev_row[j] + (*u).image_data[0][j-1] - 4*(*u).image_data[0][j] + (*u).image_data[0][j+1] + (*u).image_data[1][j]);
        (*u_bar).image_data[m-1][j] = (*u).image_data[m-1][j] + kappa*((*u).image_data[m-2][j] + (*u).image_data[m-1][j-1] - 4*(*u).image_data[m-1][j] + (*u).image_data[m-1][j+1] + next_row[j]);
      }
    }
    for(int i = 1; i < m - 1; i++){
      for(int j = 1; j < n - 1; j++){
        (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j]);
      }
    }

    // for (int i = 1; i < m-1; i++){
    //   for (int j = 1; j < n-1; j++){
    //     u_b[i][j] = u_[i][j] + kappa*(u_[i-1][j] + u_[i][j-1] - 4*u_[i][j] + u_[i][j+1] + u_[i+1][j]);
    //   }
    // }
    // for (int k = 1; k < n-1; k++){
    //   u_b[0][k]   = u_[0][k]    + kappa*(prev_row[k] + u_[0][k-1] - 4*u_[0][k] + u_[0][k+1] + u_[1][k]);
    //   u_b[m-1][k] = u_[m-1][k]  + kappa*(u_[m-2][k] + u_[m-1][k-1] - 4*u_[m-1][k] + u_[m-1][k+1] + next_row[k]);
    //   // printf("%f, %f\n",u_b[0][k],u_b[m-1][k]);
    // }
    temp.image_data = (*u).image_data;
    (*u).image_data = (*u_bar).image_data;
    (*u_bar).image_data = temp.image_data;
  }
}

void allocate_image(image *u, int m, int n){
  /*
  u sin m settes lik den lokale m'en som sendes inn i funksjonen.
  Samme med u sin n.
  Deretter allokeres minne til u sin image_data (ytre array).
  For så å allokere minne til u sine indre arrays.
  */
  u->m = m;
  u->n = n;
  u->image_data = (float **)malloc(m*sizeof(float*));
  for(int i = 0; i<m; i++){
    u->image_data[i] = (float *)malloc(n*sizeof(float));
  }
}

void deallocate_image(image *u){
  /*
  Motsatt prosess av hva som skjer i allocate_image(...) for å frigjøre minnet
  til u.
  */
  for(int i = 0; i< u->m ; i++){
    free(u->image_data[i]);
  }
  free(u->image_data);
}

void convert_jpeg_to_image(const unsigned char* image_chars, image *u){
  for (int i = 0 ; i < u->m ; i++){
    for (int j = 0 ; j < u->n ; j++){
      u->image_data[i][j] = image_chars[i*(u->n)+j];
    }
  }
}

void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  for (int i = 0 ; i < u->m ; i++){
    for (int j = 0 ; j < u->n ; j++){
      image_chars[i*(u->n)+j] = u->image_data[i][j];
    }
  }
}




// // -sende boundary info tilhverandre from index 1 til my_n/m - 1
// // -jobbe selv
// // printf("okei\n");
// for (int i = 0; i < iters ; i++){
//   // Set inner points of u_bar:
//   for (int j = 1 ; j < u->m - 1 ; j++){
//     for (int k = 1 ; k < u->n - 1 ; k++){
//       u_bar->image_data[j][k] = u->image_data[j][k] + kappa*(u->image_data[j-1][k] + u->image_data[j][k-1] - 4*u->image_data[j][k] + u->image_data[j][k+1] + u->image_data[j+1][k]);
//     }
//   }
//   // Set boundary points of u_bar (vertical):
//   for (int l = 0 ; l < u->m ; l++){
//     u_bar->image_data[l][0] = u->image_data[l][0];
//     u_bar->image_data[l][u->n-1] = u->image_data[l][u->n-1];
//   }
//   // Set boundary points of u_bar (horisontal):
//   for (int o = 0 ; o < u->n ; o++){
//     u_bar->image_data[0][o] = u->image_data[0][o];
//     u_bar->image_data[u->m-1][o] = u->image_data[u->m-1][o];
//   }
//   // Set u to be u_bar:
//   for (int p = 0 ; p < u->m ; p++){
//     for (int q = 0 ; q < u->n ; q++){
//       u->image_data[p][q] = u_bar->image_data[p][q];
//     }
//   }
// }
