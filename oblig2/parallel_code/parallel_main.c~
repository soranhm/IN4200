/* needed header files .... */
/* declarations of functions import_JPEG_file and export_JPEG_file */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../serial_code/functions.h"
#include "../simple-jpeg/import_export_jpeg.h"

int main(int argc, char *argv[]){
  int m, n, c, iters;
  int my_m, my_n, my_rank, num_procs;
  float kappa;
  image u, u_bar;
  unsigned char *image_chars, *my_image_chars;
  char *input_jpeg_filename, *output_jpeg_filename;

  int my_count, my_displacement, rest_row, my_row_count;
  int* send_recv_counts, *displacements;
  double start_time,elapsed_time,max_elapsed_time;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
  /* read from command line: kappa, iters, input_jpeg_filename, output_jpeg_file
  name */
  if (my_rank == 0){
    if (argc < 5)
    {
        printf("\033[1;31mRUN:\nmpirun -np num_procs ./parallel_main.x kappa iters input_file output_file\033[0m\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    sscanf(argv[1], "%f", &kappa);
    sscanf(argv[2], "%d", &iters);
    input_jpeg_filename = argv[3];
    output_jpeg_filename = argv[4];
    printf("Kappa = %.2f, iters = %d \n",kappa,iters);
    import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
    printf("n = %d, m = %d\n",n,m);
  }

  // Gives data to all the processors
  MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&iters, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&kappa, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  /* 2D decomposition of the m x n pixels evenly among the MPI processes */
  rest_row = m % num_procs;
  my_m = m/num_procs;
  my_n = n;

  printf("\033[1;3%dmMY_RANK = %d,  NUM_PROCS = %d\033[0m\n",my_rank+1,my_rank,num_procs );

  /* each process asks process 0 for a partitioned region */
  /* of image_chars and copy the values into u */

  printf("\033[1;3%dmSENDING ...\033[0m\n",my_rank+1);
  // If num_procs can be divided, send a bit of array to each
  if (rest_row == 0){
    my_row_count = my_m;
    my_count = my_row_count*my_n;

    // allocating space for my_image_chars
    my_image_chars = (unsigned char*)malloc((my_count)* sizeof (unsigned char));

    // Sending chunks of image_chars to different processes
    MPI_Scatter(image_chars, m*n, MPI_UNSIGNED_CHAR,
                my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);
  }

  else{ // now we have a rest
    if (my_rank < rest_row){
      my_row_count = my_m + 1;
      my_count = my_row_count*my_n;
      my_displacement = my_count*my_rank;
    }
    else{
      my_row_count = my_m;
      my_count = my_m*my_n;
      my_displacement = my_count*my_rank + rest_row*my_n;
    }
    // saves locations
    send_recv_counts = (int*)malloc(num_procs * sizeof(int));
    displacements = (int*)malloc(num_procs * sizeof(int));

    // collectiong the chunk of array
    MPI_Allgather(&my_count, 1, MPI_INT, send_recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&my_displacement, 1, MPI_INT, displacements, 1, MPI_INT, MPI_COMM_WORLD);

    // allocating space for my_image_chars
    my_image_chars = (unsigned char*)malloc((my_count) * sizeof (unsigned char));

    // Sending chunks of image_chars to different processes
    MPI_Scatterv(image_chars, send_recv_counts, displacements, MPI_UNSIGNED_CHAR,
                my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);
  }

  printf("\033[1;3%dmmy_m = %d, my_n = %d\033[0m\n",my_rank+1,my_row_count,my_n);
  // Put the data into image structures
  printf("\033[1;3%dmALLOCATING IMAGE ...\033[0m\n",my_rank+1);
  allocate_image(&u, my_row_count, my_n);
  allocate_image(&u_bar, my_row_count, my_n);
  printf("\033[1;3%dmCONVERTING JPG TO IMAGE ...\033[0m\n",my_rank+1);
  convert_jpeg_to_image(my_image_chars, &u);

  // Starting calculations
  printf("\033[1;3%dmWAITING FOR ALL PROCESSORS\033[0m\n",my_rank+1);
  MPI_Barrier(MPI_COMM_WORLD); // waiting for all processes to collect
  start_time = MPI_Wtime();

  printf("\033[1;3%dmISO DIFFUSION DENOISING PARALLEL ...\033[0m\n",my_rank+1);
  iso_diffusion_denoising_parallel(&u, &u_bar, kappa, iters);

  elapsed_time = MPI_Wtime() - start_time;

  printf("\033[1;3%dmCONVERTING IMAGE TO JPG ...\033[0m\n",my_rank+1);
  convert_image_to_jpeg(&u_bar, my_image_chars);

  deallocate_image(&u);
  deallocate_image(&u_bar);

  /* each process sends its resulting content of u_bar to process 0 */
  /* process 0 receives from each process incoming values and */
  /* copy them into the designated region of struct whole_image */

  printf("\033[1;3%dmGATHERING ...\033[0m\n",my_rank+1);
  if (rest_row == 0){
    MPI_Gather(my_image_chars, my_count, MPI_UNSIGNED_CHAR,
              image_chars, m*n, MPI_UNSIGNED_CHAR,
              0, MPI_COMM_WORLD);
  }
  else{
    MPI_Gatherv(my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                image_chars, send_recv_counts, displacements, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);

    free(send_recv_counts);
    free(displacements);
  }

  free(my_image_chars);

  // Takes an array of input elements on each process and returns an
  // array of output elements to the root process
  // Collecting all the time to the root
  MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  printf("\033[1;3%dmMY_RANK: %d  -->  DONE ...\033[0m\n",my_rank+1,my_rank);

  // Write result
  if (my_rank == 0){
    export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);
    printf("\033[1;36mTime : %lf s -> iters = %d, kappa = %f \033[0m\n", max_elapsed_time,iters,kappa);
  }
  MPI_Finalize();
  //printf("%s\n", );
  return 0;
}
