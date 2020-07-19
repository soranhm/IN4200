#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
// make use of two functions from the simplejpeg library
void import_JPEG_file(const char *filename, unsigned char **image_chars, int *image_height, int *image_width, int *num_components);
void export_JPEG_file(const char *filename, unsigned char *image_chars, int image_height, int image_width, int num_components, int quality);


typedef struct image{
  float** image_data;  /* a 2D array of floats */
  int m;             /* # pixels in x-direction */
  int n;             /* # pixels in y-direction */
}image;

//Allocate the 2D array image_data inside u, when m and n are given as input
void allocate_image(image *u, int m, int n, int my_rank, int num_procs){
  (*u).m = m;
  (*u).n = n;

  if(my_rank == -1){    //whole_image
    (*u).image_data = (float**)malloc((*u).m*sizeof(float*));
    for (int i = 0; i < (*u).m; i++){
      (*u).image_data[i] = (float*)malloc((*u).n*sizeof(float));
    }
  }
  else{
    (*u).image_data = (float**)malloc((m + 2)*sizeof(float*));
    for (int i = 0; i < (m + 2); i++){
      (*u).image_data[i] = (float*)malloc(n*sizeof(float));
    }
  }
}

//Free the storage used by the 2D array image_data inside u
void deallocate_image(image *u, int my_rank){
  if(my_rank == -1 ){    //whole image
    for(int i = 0; i < (*u).m; i++){
      free((*u).image_data[i]);
    }
    free((*u).image_data);
  }
  else{
    for(int i = 0; i < ((*u).m + 2); i++){
      free((*u).image_data[i]);
    }
    free((*u).image_data);
  }
}

//convert a 1D array of unsigned char values into an image struct
void convert_jpeg_to_image(const unsigned char* image_chars, image *u, int my_rank){
  int i, j, index;
  index = 0;
  for(i = 0; i < (*u).m; i++){
    for(j = 0; j < (*u).n; j++){
      (*u).image_data[i+1][j] = (float)image_chars[index++];
    }
  }
}

//Convert an image struct into an !D array of unsigned chars
void convert_image_to_jpeg(const image *u, unsigned char* image_chars){
  int index = 0;
  int i,j;

  for(i = 0; i < (*u).m; i++){
    for(j = 0; j < (*u).n; j++){
      image_chars[index] = (unsigned char)(*u).image_data[i][j];
      index++;
    }
  }
}


void iso_diffusion_denoising_parallel(image *u, image *u_bar, float kappa, int iters, int my_rank, int num_procs){
  int i,j,k;
  image u_temp;
  int m = (*u).m;
  int n = (*u).n;

  for(k = 0; k < iters; k++){
    if(my_rank == 0){
      MPI_Send(&(*u).image_data[m][0], n, MPI_FLOAT, my_rank + 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&(*u).image_data[m+1][0], n, MPI_FLOAT, my_rank + 1, 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
      for(i = 2; i < (*u).m+1; i++){
        for(j = 1; j < (*u).n - 1; j++){
          (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j]);
          if(i == 2 && j == 2){
          }
        }
      }
    }
    else if(my_rank == num_procs - 1){
      MPI_Recv(&(*u).image_data[0][0], n, MPI_FLOAT, my_rank - 1, 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
      MPI_Send(&(*u).image_data[1][0], n, MPI_FLOAT, my_rank - 1, 0, MPI_COMM_WORLD);

      for(i = 1; i < (*u).m; i++){
        for(j = 1; j < (*u).n - 1; j++){
          (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j]);
        }
      }
    }
    else{
      MPI_Recv(&(*u).image_data[0][0], n, MPI_FLOAT, my_rank - 1, 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
      MPI_Send(&(*u).image_data[1][0], n, MPI_FLOAT, my_rank - 1, 0, MPI_COMM_WORLD);
      MPI_Send(&(*u).image_data[m][0], n, MPI_FLOAT, my_rank + 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&(*u).image_data[m+1][0], n, MPI_FLOAT, my_rank + 1, 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);

      for(i = 1; i < (*u).m+1; i++){
        for(j = 1; j < (*u).n - 1; j++){
          (*u_bar).image_data[i][j] = (*u).image_data[i][j] + kappa*((*u).image_data[i-1][j] + (*u).image_data[i][j-1] - 4*(*u).image_data[i][j] + (*u).image_data[i][j+1] + (*u).image_data[i+1][j]);
        }
      }
    }

    u_temp.image_data = (*u).image_data;
    (*u).image_data = (*u_bar).image_data;
    (*u_bar).image_data = u_temp.image_data;

    MPI_Barrier(MPI_COMM_WORLD);
  }
}



int main(int argc, char *argv[])
{
  int m, n, c, iters;
  int my_m, my_n, my_rank, num_procs;
  float kappa;
  image u, u_bar, whole_image;
  unsigned char *image_chars, *my_image_chars;
  char *input_jpeg_filename, *output_jpeg_filename;


  MPI_Init (&argc, &argv);                    //Initialize the MPI execution environment
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);   //Determines the rank of the calling process in the communicator
  MPI_Comm_size (MPI_COMM_WORLD, &num_procs); //Determines the size of the group associated with a communicator.
                                              //Returns the total number of MPI processes in the specified communicator

  //read from command line: kappa, iters, input_jpeg_filename, output_jpeg_filename
  iters = atof(argv[1]);
  kappa = atof(argv[2]);
  input_jpeg_filename = argv[3];
  output_jpeg_filename = argv[4];

  if (my_rank==0) {
    import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
    allocate_image(&whole_image, m, n, -1, num_procs);
  }

  //Broadcasts a message from the process with rank "root" to all other processes of the communicator
  MPI_Bcast (&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  /* divide the m x n pixels evenly among the M  printf("my_rank : %d u.m : %d u.n: %d size of my_image_chars: %d \n",my_rank,u.m,u.n, size );
  PI processes */
  int rows_per_process = m/num_procs;
  int rest = m%num_procs;
  if(my_rank == 0){
    my_m = rest + rows_per_process;
  }
  else{
    my_m = rows_per_process;
  }
  my_n = n;
  int pixels_per_process = rows_per_process*my_n;

  allocate_image (&u, my_m, my_n, my_rank, num_procs);
  allocate_image (&u_bar, my_m, my_n, my_rank, num_procs);

  //allocate space for my_image_chars
  int size = my_m*my_n;
  my_image_chars = (unsigned char*)malloc(size*sizeof(unsigned char));

  MPI_Barrier (MPI_COMM_WORLD);
  /* each process asks process 0 for a partitioned region */
  /* of image_chars and copy the values into u */
  /*  ...  */

  if(my_rank > 0){
    MPI_Recv(my_image_chars, pixels_per_process, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
    convert_jpeg_to_image (my_image_chars, &u, my_rank);
  }
  else{
    int i,j;
    for(i = 1; i < num_procs; i++){
      int index = rest + (pixels_per_process * i);
      MPI_Send(&image_chars[index], pixels_per_process, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
    }
    for(j = 0; j < size; j++){
      my_image_chars[j] = image_chars[j];
    }
    convert_jpeg_to_image (my_image_chars, &u, my_rank);
  }

  (u).m = my_m;
  (u).n = my_n;
  (u_bar).m = my_m;
  (u_bar).n = my_n;

  iso_diffusion_denoising_parallel(&u, &u_bar, kappa, iters, my_rank, num_procs);

  /* each process sends its resulting content of u_bar to process 0 */
  /* process 0 receives from each process incoming values and */
  /* copy them into the designated region of struct whole_image */
  /*  ...  */

  if(my_rank == 0){
    int index = rest + rows_per_process - 1;
    int i,j;
    for(i = 1; i < num_procs; i++){
      for(j = 0; j < rows_per_process; j++){
          MPI_Recv(&whole_image.image_data[index + j][0], n, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      index += rows_per_process;
    }
    for(i = 1; i < my_m; i++){
      for(j = 0; j < my_n; j++){
        whole_image.image_data[i][j] = u.image_data[i+1][j];
      }
    }
  }
  else{
    int i;
    for(i = 0; i < my_m; i++){
      MPI_Send(&u.image_data[i+1][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
  }

  if (my_rank == 0){
    convert_image_to_jpeg(&whole_image, image_chars);
    export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);
    deallocate_image (&whole_image, -1);
  }

  free(my_image_chars);

  deallocate_image (&u, my_rank);
  deallocate_image (&u_bar, my_rank);


  MPI_Finalize ();
  return 0;
}
