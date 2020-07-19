#include "../simple-jpeg/import_export_jpeg.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "functions.h"

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
  for (int i = 0; i < m; i++) {
    u->image_data[i] = (float*)malloc(n * sizeof(float));
  }
}


void deallocate_image(image *u){
  /*
  The purpose of function deallocate image is to free the storage
  used by the 2D array image data inside u.
  */
  //deallocate all m*n vectors
  for (int i = 0; i < u->m; i++) {
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
  for (int i = 0; i < u->m; i++) {
    for (int j = 0; j < u->n; j++) {
      u->image_data[i][j] = (float)image_chars[i*u->n + j];
    }
  }
}


void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  /*
  Function convert image to jpeg does the conversion in
  the opposite direction.
  */

  // Converting 2D back float back to 1D unsigned char
  for (int i = 0; i < u->m; i++) {
    for (int j = 0; j < u->n; j++) {
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
  for(int i = 0; i < u->m ;i++){
    u_bar->image_data[i][0] = u->image_data[i][0];
    u_bar->image_data[i][u->n-1] = u->image_data[i][u->n-1];
  }

  //top and bottom boundary
  for(int j = 0; j < u->n ;j++) {
    u_bar->image_data[0][j] = u->image_data[0][j];
    u_bar->image_data[u->m-1][j] = u->image_data[u->m-1][j];
  }

  for(int k = 0; k < iters; k++) {
    for (int i = 1; i < u->m-1; i++) {
      for (int j = 1; j < u->n-1; j++) {
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
}

// The funsjon i use i parallel_main.c
void iso_diffusion_denoising_parallel(image *u, image *u_bar, float kappa, int iters){
  printf("iso_diffusion_denoising_parallel\n");
}
