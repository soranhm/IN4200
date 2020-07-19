
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include </home/cyclone/INF3380/ME01/simple-jpeg/import_export_jpeg.h>

/* Structure for image information. Relative to the template, m
and n are substituted with image_height and image_width respectively.*/
typedef struct{
	float** image_data;
	int image_height;
	int image_width;
} image;

/* Defines height and width for u (of type image) as well as
Allocating 2D-array u.image_data for future storage of image.*/
void allocate_image(image* u, int m, int n){
	int i;
	u->image_height = m;
	u->image_width = n;
	u->image_data = (float**)malloc(m * sizeof(float*));

	for (i = 0; i < m; i++){
		u->image_data[i] = (float*)malloc(n * sizeof(float));
	}
}

/* Deallocates the 2D-array u.image_data and frees the memory.*/
void deallocate_image(image* u){
	int i, j;

	for (i = 0; i < u->image_height; i++){
		free(u->image_data[i]);
	}

	free(u->image_data);
}

/* Converts 1D-array image_chars (of type unsigned char) into
2D-array u.image_data (of type float) so computations can be done.*/
void convert_jpeg_to_image(const unsigned char* image_chars, image* u){
	int i, j;

	for (i = 0; i < u->image_height; i++){
		for (j = 0; j < u->image_width; j++){
			u->image_data[i][j] = (float)image_chars[i * u->image_width + j];		}
	}
}

/* Converts 2D-array u.image_data (of type float) back to 1D-array
image_chars (of type unsigned char) so image can be exported to JPEG.*/
void convert_image_to_jpeg(const image* u, unsigned char* image_chars){
	int temp_int, i, j;

	for (i = 0; i < u->image_height; i++){
		for (j = 0; j < u->image_width; j++){
			temp_int = (int)((u->image_data[i][j]) + 0.5); // Round to int.
			image_chars[i * u->image_width + j] = (unsigned char)temp_int;
		}
	}
}

/* Uses isotropic diffusion in interior of u iters times to create smoothed
image u_bar. On the boundaries of u, values of u are copied in to u_bar.*/
void iso_diffusion_denoising(image* u, image* u_bar, float kappa, int iters){
	int i, j, k;

	// Vertical boundary of image (copying values).
	for (i = 0; i < u->image_height; i++){
		u_bar->image_data[i][0] = u->image_data[i][0];
		u_bar->image_data[i][u_bar->image_width - 1]
			= u->image_data[i][u->image_width - 1];
	}

	/* Horizontal boundary of image (copying values). Start at j = 1
	and end at N - 2 becuse the corners are already taken care of above.*/
	for (j = 1; j < (u->image_width - 1); j++){
		u_bar->image_data[0][j] = u->image_data[0][j];
		u_bar->image_data[u_bar->image_height - 1][j]
			= u->image_data[u->image_height - 1][j];
	}

	// Isotropic diffusion iters times in the interior of image.
	for (k = 0; k < iters; k++){
		for (i = 1; i < (u->image_height - 1); i++){
			for (j = 1; j < (u->image_width - 1); j++){
				u_bar->image_data[i][j] = u->image_data[i][j]
					+ kappa * (u->image_data[i - 1][j]
					+ u->image_data[i][j - 1]
					- 4 * u->image_data[i][j]
					+ u->image_data[i][j + 1]
					+ u->image_data[i + 1][j]);
			}
		}

		// Setting u = u_bar before new iteration of diffusion.
		for (i = 1; i < (u->image_height - 1); i++){
			for (j = 1; j < (u->image_width - 1); j++){
				u->image_data[i][j] = u_bar->image_data[i][j];
			}
		}

	}
}

/* Main function based on skeleton in template that takes command-line
arguments iters, kappa, input_jpeg_filename, output_jpeg_filename, and
calls the above defined functions to execute the image denoising process.*/
int main(int argc, char* argv[]){
	image u, u_bar;							// Type struct image.
	int image_height, image_width, num_comp;
	unsigned char* image_chars;
	int iters = atoi(argv[1]);				// String to int.
	float kappa = atof(argv[2]);			// String to float.
	char* input_jpeg_filename = argv[3];
	char* output_jpeg_filename = argv[4];

	import_JPEG_file(input_jpeg_filename, &image_chars,		// Get JPEG.
		&image_height, &image_width, &num_comp);

	printf("W: %d, H: %d, C: %d, Name: %s\n", image_width,	// Image info.
		image_height, num_comp, input_jpeg_filename);

	allocate_image(&u, image_height, image_width);			// Allocate memory.
	allocate_image(&u_bar, image_height, image_width);		// Allocate memory.

	convert_jpeg_to_image(image_chars, &u);					// image_chars -> u.
	iso_diffusion_denoising(&u, &u_bar, kappa, iters);		// Remove noise.

	convert_image_to_jpeg(&u_bar, image_chars);				// u_bar -> image_chars.
	export_JPEG_file(output_jpeg_filename, image_chars,		// Create new JPEG.
		image_height, image_width, num_comp, 75);

	deallocate_image(&u);									// Free memory.
	deallocate_image(&u_bar);								// Free memory.

	return(0);
}
