#include <stdio.h>
#include <stdlib.h>


typedef struct image{
  float** image_data;  /* a 2D array of floats */
  int m;             /* # pixels in x-direction */
  int n;             /* # pixels in y-direction */
}image;

//Allocate the 2D array image_data inside u, when m and n are given as input
void allocate_image(image *u, int m, int n){
  (*u).image_data = (float**)malloc((*u).m*sizeof(float*));
  for (int i = 0; i < (*u).m; i++){
    (*u).image_data[i] = (float*)malloc((*u).n*sizeof(float));
  }
}

//Free the storage used by the 2D array image_data inside u
void deallocate_image(image *u){
  for(int i = 0; i < (*u).m; i++){
    free((*u).image_data[i]);
  }
  free((*u).image_data);
}

//convert a 1D array of unsigned char values into an image struct
void convert_jpeg_to_image(const unsigned char* image_chars, image *u){
  printf("HEI\n");
  int index = 0;
  int i, j;
  for(i = 0; i < (*u).m; i++){
    for(j = 0; j < (*u).n; j++){
      (*u).image_data[i][j] = (float)image_chars[index];
      index++;
    }
  }
  for(i = 500; i < 1000; i++){
    for(j = 500; j < 1000; j++){
      (*u).image_data[i][j] = (float)0;
    }
  }
}

//Convert an image struct into an !D array of unsigned chars
void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  int index = 0;
  int i,j;
  int size = (*u).m*(*u).n;
  image_chars = (unsigned char*)malloc(sizeof((*u).m)*sizeof((*u).n));
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      image_chars[index] = (unsigned char)(*u).image_data[i][j];
      index++;
    }
  }
}


void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters){
  int m = (*u).m;
  int n = (*u).n;
  int i,j,k;

  allocate_image(u_bar, m, n);

  for(k = 0; k < iters; k++){
    for(i = 1; i < m - 1; i++){
      for(j = 1; j < n - 1; j++){
        (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j+1]);
        //printf((*u_bar).image_data[i][j]);
      }
    }
  }
}
