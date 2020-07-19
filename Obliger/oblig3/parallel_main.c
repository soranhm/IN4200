#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

typedef struct
{
    float** image_data; // 2D array of floats
    int m;              // Number of pixels in y-direction
    int n;              // Number of pixels in x-direction
}
Image;

// Make use of two functions from the simplejpeg library
void import_JPEG_file(const char* filename, unsigned char** image_chars,
                      int* image_height, int* image_width,
                      int* num_components);

void export_JPEG_file(const char* filename, unsigned char* image_chars,
                      int image_height, int image_width,
                      int num_components, int quality);

// Declare local functions
void allocate_image(Image* u, int m, int n);
void deallocate_image(Image* u);
void convert_jpeg_to_image(const unsigned char* image_chars, Image* u);
void convert_image_to_jpeg(const Image* u, unsigned char* image_chars);
void iso_diffusion_denoising_parallel(Image* u, Image* u_bar, float kappa, int iters);

void allocate_image(Image* u, int m, int n)
{
    float* data_array; // Array for contiguous values
    int i;

    // Allocate memory for all values
    data_array = calloc(m*n, sizeof(float));

    // Allocate memory for pointers to the rows
    u->image_data = malloc(m*sizeof(float*));

    if (!data_array || !u->image_data)
    {
        printf("Error: allocation failed\n");
        exit(1);
    }

    // Fill array of pointers with row addresses
    for (i = 0; i < m; i++)
    {
        u->image_data[i] = data_array + n*i;
    }

    // Set image dimensions
    u->m = m;
    u->n = n;
}

void deallocate_image(Image* u)
{
    // Deallocate values
    free(u->image_data[0]);

    // Deallocate pointers
    free(u->image_data);
    u->image_data = NULL;

    // Clear image dimensions
    u->m = 0;
    u->n = 0;
}

void convert_jpeg_to_image(const unsigned char* image_chars, Image* u)
{
    int i, j;

    // Fill 2D float array with converted values from 1D char array
    for (i = 0; i < u->m; i++) {
        for (j = 0; j < u->n; j++)
        {
            u->image_data[i][j] = (float)image_chars[i*u->n + j];
        }
    }
}

void convert_image_to_jpeg(const Image* u, unsigned char* image_chars)
{
    int i, j;

    // Fill 1D char array with converted values from 2D float array
    for (i = 0; i < u->m; i++) {
        for (j = 0; j < u->n; j++)
        {
            image_chars[i*u->n + j] = (unsigned char)u->image_data[i][j];
        }
    }
}

void iso_diffusion_denoising_parallel(Image* u, Image* u_bar, float kappa, int iters)
{
    int i, j, k;
    int m, n;
    float** u_dat;
    float** u_bar_dat;
    float** temp;
    float* row_before;
    float* row_after;
    int last_row_beg;
    int my_rank, n_procs, next_rank, prev_rank;

    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &n_procs);

    // Find previous and next my_rank (circular shift)
    prev_rank = (my_rank - 1 + n_procs) % n_procs;
    next_rank = (my_rank + 1) % n_procs;

    // Store image content and dimensions in local variables
    m = u->m;
    n = u->n;
    u_dat = u->image_data;
    u_bar_dat = u_bar->image_data;

    // Allocate memory for ghost rows
    row_before = malloc(n*sizeof(float));
    row_after = malloc(n*sizeof(float));

    last_row_beg = (m - 1)*n;

    // Perform iterations
    for (k = 0; k < iters; k++) {

        // Send first local row to previous my_rank, and recieve the row
        // following the last local row from the next my_rank
        MPI_Sendrecv(u_dat[0], n, MPI_FLOAT, prev_rank, 0,
                     row_after, n, MPI_FLOAT, next_rank, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Send last local row to next my_rank, and recieve the row
        // preceding the first local row from the previous my_rank
        MPI_Sendrecv(u_dat[0] + last_row_beg, n, MPI_FLOAT, next_rank, 1,
                     row_before, n, MPI_FLOAT, prev_rank, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Perform denoising on first and last local row
        for (j = 1; j < n-1; j++)
        {
            u_bar_dat[0][j] = u_dat[0][j] + kappa*(  row_before[j]
                                                   + u_dat[0][j-1]
                                                   - 4*u_dat[0][j]
                                                   + u_dat[0][j+1]
                                                   + u_dat[1][j]);

            u_bar_dat[m-1][j] = u_dat[m-1][j] + kappa*(  u_dat[m-2][j]
                                                       + u_dat[m-1][j-1]
                                                       - 4*u_dat[m-1][j]
                                                       + u_dat[m-1][j+1]
                                                       + row_after[j]);
        }

        // Perform denoising on the remaining rows
        for (i = 1; i < m-1; i++) {
            for (j = 1; j < n-1; j++)
            {
                u_bar_dat[i][j] = u_dat[i][j] + kappa*(  u_dat[i-1][j]
                                                       + u_dat[i][j-1]
                                                       - 4*u_dat[i][j]
                                                       + u_dat[i][j+1]
                                                       + u_dat[i+1][j]);
            }
        }

        // Swap pointers
        temp = u_dat;
        u_dat = u_bar_dat;
        u_bar_dat = temp;
    }
}

int main(int argc, const char* argv[])
{
    int m, n, c, iters;
    int my_count, my_displacement, my_rank, n_procs;
    int my_even_row_count, n_remaining_rows, my_row_count;
    int i;
    float kappa;
    Image u, u_bar;
    unsigned char* image_chars;
    unsigned char* my_image_chars;
    char* input_jpeg_filename;
    char* output_jpeg_filename;
    int* send_recv_counts;
    int* displacements;
    double start_time, elapsed_time, max_elapsed_time;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &n_procs);

    if (my_rank == 0)
    {
        // Read from command line
        if (argc < 5)
        {
            printf("Usage:\nmpiexec -np n_procs ./parallel_main.x n_iterations kappa input_file output_file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        sscanf(argv[1], "%d", &iters);
        sscanf(argv[2], "%f", &kappa);
        input_jpeg_filename = argv[3];
        output_jpeg_filename = argv[4];

        // Echo input
        printf("Processes: %d\nIterations: %d\nKappa: %f\nInput file: %s\nOutput file: %s\n",
               n_procs, iters, kappa, input_jpeg_filename, output_jpeg_filename);

        import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
    }

    // Distribute input data to all processes
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iters, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&kappa, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Divide the m x n pixels evenly among the MPI processes
    my_even_row_count = m/n_procs;
    n_remaining_rows = m % n_procs;

    if (n_remaining_rows == 0)
    {
        my_row_count = my_even_row_count;
        my_count = my_row_count*n;

        // Allocate memory for a local image section
        my_image_chars = calloc(my_count, sizeof(unsigned char));

        // Scatter rows of the full image
        MPI_Scatter(image_chars, m*n, MPI_UNSIGNED_CHAR,
                    my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                    0, MPI_COMM_WORLD);
    }
    else
    {
        if (my_rank < n_remaining_rows)
        {
            my_row_count = my_even_row_count + 1;
            my_count = my_row_count*n;
            my_displacement = my_count*my_rank;
        }
        else
        {
            my_row_count = my_even_row_count;
            my_count = my_row_count*n;
            my_displacement = my_count*my_rank + n_remaining_rows*n;
        }

        send_recv_counts = malloc(n_procs*sizeof(int));
        displacements = malloc(n_procs*sizeof(int));

        MPI_Allgather(&my_count, 1, MPI_INT, send_recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Allgather(&my_displacement, 1, MPI_INT, displacements, 1, MPI_INT, MPI_COMM_WORLD);

        // Allocate memory for a local image section
        my_image_chars = calloc(my_count, sizeof(unsigned char));

        // Scatter rows of the full image
        MPI_Scatterv(image_chars, send_recv_counts, displacements, MPI_UNSIGNED_CHAR,
                     my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                     0, MPI_COMM_WORLD);
    }

    // Put the data into image structures
    allocate_image(&u, my_row_count, n);
    allocate_image(&u_bar, my_row_count, n);
    convert_jpeg_to_image(my_image_chars, &u);

    // Perform denoising

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    iso_diffusion_denoising_parallel(&u, &u_bar, kappa, iters);

    elapsed_time = MPI_Wtime() - start_time;

    // Put the data into char arrays
    convert_image_to_jpeg(&u_bar, my_image_chars);
    deallocate_image(&u);
    deallocate_image(&u_bar);

    // Gather the processed image sections
    if (n_remaining_rows == 0)
    {
        MPI_Gather(my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                   image_chars, m*n, MPI_UNSIGNED_CHAR,
                   0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Gatherv(my_image_chars, my_count, MPI_UNSIGNED_CHAR,
                    image_chars, send_recv_counts, displacements, MPI_UNSIGNED_CHAR,
                    0, MPI_COMM_WORLD);

        free(send_recv_counts);
        free(displacements);
    }
    free(my_image_chars);

    MPI_Reduce(&elapsed_time, &max_elapsed_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Write result
    if (my_rank == 0)
    {
        export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);

        printf("Elapsed time: %lf s\n", max_elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
