#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// make use of two functions from the simplejpeg library
void import_JPEG_file(const char *filename, unsigned char **image_chars, int *image_height, int *image_width, int *num_components);
void export_JPEG_file(const char *filename, unsigned char *image_chars,int image_height, int image_width,int num_components, int quality);


typedef struct image{
  float** image_data;  /* a 2D array of floats */
  int m;             /* # pixels in x-direction */
  int n;             /* # pixels in y-direction */
}image;

//Allocate the 2D array image_data inside u, when m and n are given as input
void allocate_image(image *u, int m, int n){
  (*u).m = m;
  (*u).n = n;
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
  int index = 0;
  int i, j;
  for(i = 0; i < (*u).m; i++){
    for(j = 0; j < (*u).n; j++){
      (*u).image_data[i][j] = (float)image_chars[index];
      index++;
    }
  }
}

//Convert an image struct into an !D array of unsigned chars
void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  int index = 0;
  int i,j;
  for(i = 0; i < u->m; i++){
    for(j = 0; j < (*u).n; j++){
      image_chars[index] = (unsigned char)(*u).image_data[i][j];
      index++;
    }
  }
}


void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters){
  int i,j,k;

  image u_temp;

  for(k = 0; k < iters; k++){
    for(i = 1; i < (*u).m - 1; i++){
      for(j = 1; j < (*u).n - 1; j++){
        (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j]);
      }
    }

    u_temp.image_data = (*u).image_data;
    (*u).image_data = (*u_bar).image_data;
    (*u_bar).image_data = u_temp.image_data;
  }
}




int main(int argc, char *argv[]){
  int m, n, c, iters;
  float kappa;
  image u, u_bar;
  unsigned char *image_chars;
  char *input_jpeg_filename, *output_jpeg_filename;
  /* read from command line: kappa, iters, input_jpeg_filename, output_jpeg_filename  l */
  /* ... */

  iters = atof(argv[1]);
  kappa = atof(argv[2]);
  input_jpeg_filename = argv[3];
  output_jpeg_filename = argv[4];


  import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
  allocate_image (&u, m, n);
  allocate_image (&u_bar, m, n);
  convert_jpeg_to_image (image_chars, &u);
  iso_diffusion_denoising (&u, &u_bar, kappa, iters);
  convert_image_to_jpeg (&u, image_chars);
  export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);
  deallocate_image (&u);
  deallocate_image (&u_bar);
  return 0;
}
