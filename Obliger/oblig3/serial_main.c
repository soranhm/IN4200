#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
void iso_diffusion_denoising(Image* u, Image* u_bar, float kappa, int iters);

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

void iso_diffusion_denoising(Image* u, Image* u_bar, float kappa, int iters)
{
    int i, j, k;
    int m, n;
    float** up;
    float** up_bar;
    float** temp;

    // Store image content and dimensions in local variables
    m = u->m;
    n = u->n;
    up = u->image_data;
    up_bar = u_bar->image_data;

    // Perform iterations
    for (k = 0; k < iters; k++)
    {
        for (i = 1; i < m-1; i++) {
            for (j = 1; j < n-1; j++)
            {
                up_bar[i][j] = up[i][j] + kappa*(  up[i-1][j]
                                                 + up[i][j-1]
                                                 - 4*up[i][j]
                                                 + up[i][j+1]
                                                 + up[i+1][j]);
            }
        }

        // Swap pointers
        temp = up;
        up = up_bar;
        up_bar = temp;
    }
}

int main(int argc, const char* argv[])
{
    int m, n, c, iters;
    float kappa;
    Image u, u_bar;
    unsigned char* image_chars;
    char* input_jpeg_filename;
    char* output_jpeg_filename;
    clock_t start_clock, end_clock;

    // Read from command line
    if (argc < 5)
    {
        printf("Usage:\n./serial_main.x n_iterations kappa input_file output_file\n");
        exit(1);
    }
    sscanf(argv[1], "%d", &iters);
    sscanf(argv[2], "%f", &kappa);
    input_jpeg_filename = argv[3];
    output_jpeg_filename = argv[4];

    // Echo input
    printf("Iterations: %d\nKappa: %f\nInput file: %s\nOutput file: %s\n",
           iters, kappa, input_jpeg_filename, output_jpeg_filename);

    // Get input image

    import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);

    allocate_image(&u, m, n);
    allocate_image(&u_bar, m, n);

    convert_jpeg_to_image(image_chars, &u);

    // Perform denoising

    start_clock = clock();

    iso_diffusion_denoising (&u, &u_bar, kappa, iters);

    end_clock = clock();

    // Write output image

    convert_image_to_jpeg(&u_bar, image_chars);

    deallocate_image(&u);
    deallocate_image(&u_bar);

    export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);

    printf("Elapsed time: %lf s\n", (double)(end_clock - start_clock)/CLOCKS_PER_SEC);

    return 0;
}
