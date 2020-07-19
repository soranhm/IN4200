
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <mpi.h>
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
			u->image_data[i][j] = (float)image_chars[i * u->image_width + j];
		}
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
void IDDP(image* u, image* u_bar, float kappa, int iters, int num_procs, int my_rank){
	int length_bound = u->image_width - 2; // No need to exhange egde values.
	float* my_lower_neighbour = (float*)malloc(length_bound * sizeof(float));
	float* my_upper_neighbour = (float*)malloc(length_bound* sizeof(float));
	MPI_Status status;
	int i, j, k;

	// Vertical boundary of image (copying values as in serial version).
	for (i = 0; i < u->image_height; i++){
		u_bar->image_data[i][0] = u->image_data[i][0];
		u_bar->image_data[i][u_bar->image_width - 1]
			= u->image_data[i][u->image_width - 1];
	}

	/* Horizontal boundary of image (copying values). Start at j = 1 and
	end at N - 2 becuse the corners are already taken care of above. The
	Horizontal boundaries in the interior regions are not boundaries of
	the whole image; only region process 0 and process (num_procs - 1) are.*/
	if (my_rank == 0 || my_rank == (num_procs - 1)){
		for (j = 1; j < (u->image_width - 1); j++){
			u_bar->image_data[0][j] = u->image_data[0][j];
			u_bar->image_data[u_bar->image_height - 1][j]
				= u->image_data[u->image_height - 1][j];
		}
	}

	// Isotropic diffusion iters times in the interior of image.
	for (k = 0; k < iters; k++){
		/* Need to communicate boundaries between regions operated on by
		different processors so the formula will work on the boundaries. There
		is no need to impose any MPI_Barrier() in this function because the
		processes will implicitly be synchronized by the Send/Recv operations.*/

		/* The "end-regions" of the image, process 0 and (num_process - 1),
		only needs to send values from one boundary and receive one boundary.*/
		if (my_rank == 0){
			MPI_Send(&u->image_data[u->image_height - 1][1],
				length_bound, MPI_FLOAT, 1, 12, MPI_COMM_WORLD);
			MPI_Recv(my_upper_neighbour, length_bound,
				MPI_FLOAT, 1, 12, MPI_COMM_WORLD, &status);
		}

		/* The internal regions of the image needs to send and receive values
		from both the upper and lower boundary. Odd-num regions starts receiving.*/
		else if (my_rank != 0 && my_rank != (num_procs - 1)){
			if (my_rank % 2 == 1){
				MPI_Recv(my_lower_neighbour, length_bound, MPI_FLOAT,
					my_rank - 1, 12, MPI_COMM_WORLD, &status);
				MPI_Send(&u->image_data[u->image_height - 1][0],
					length_bound, MPI_FLOAT, my_rank + 1, 12, MPI_COMM_WORLD);
				MPI_Recv(my_upper_neighbour, length_bound, MPI_FLOAT,
					my_rank + 1, 12, MPI_COMM_WORLD, &status);
				MPI_Send(&u->image_data[0][0], length_bound,
					MPI_FLOAT, my_rank - 1, 12, MPI_COMM_WORLD);
			}

			/* Even numbered regions starts sending (and then receives).*/
			else {
				MPI_Send(&u->image_data[u->image_height - 1][0],
					length_bound, MPI_FLOAT, my_rank + 1, 12, MPI_COMM_WORLD);
				MPI_Recv(my_lower_neighbour, length_bound, MPI_FLOAT,
					my_rank - 1, 12, MPI_COMM_WORLD, &status);
				MPI_Send(&u->image_data[0][0], length_bound,
					MPI_FLOAT, my_rank - 1, 12, MPI_COMM_WORLD);
				MPI_Recv(my_upper_neighbour, length_bound, MPI_FLOAT,
					my_rank + 1, 12, MPI_COMM_WORLD, &status);
			}
		}

		/* Upper "end-regions" of image. Only
		exhanges values along one boundary.*/
		else {
			MPI_Recv(my_lower_neighbour, length_bound, MPI_FLOAT,
				my_rank - 1, 12, MPI_COMM_WORLD, &status);
			MPI_Send(&u->image_data[0][0], length_bound,
				MPI_FLOAT, my_rank - 1, 12, MPI_COMM_WORLD);
		}

		/* Iso diffusion for all interior points in each region.*/
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

		/* Diffusion on lower boundary for each region requires
		use of my_lower_neighbour in the diffusion formula.*/
		if (my_rank != 0){
			for (j = 1; j < (u->image_width - 1); j++){
				u_bar->image_data[0][j] = u->image_data[0][j]
					+ kappa * (my_lower_neighbour[j]
					+ u->image_data[0][j - 1]
					- 4 * u->image_data[0][j]
					+ u->image_data[0][j + 1]
					+ u->image_data[1][j]);
			}
		}

		/* Diffusion on upper boundary for each region requires
		use of my_upper_neighbour in the diffusion formula.*/
		if (my_rank != (num_procs - 1)){
			for (j = 1; j < (u->image_width - 1); j++){
				u_bar->image_data[u->image_height - 1][j]
					= u->image_data[u->image_height - 1][j]
					+ kappa * (u->image_data[u->image_height - 2][j]
					+ u->image_data[u->image_height - 1][j - 1]
					- 4 * u->image_data[u->image_height - 1][j]
					+ u->image_data[u->image_height - 1][j + 1]
					+ my_upper_neighbour[j]);
			}
		}

		/* Setting u = u_bar before new iteration of diffusion. Not
		necessary to update vertical egdes as they stays constant.*/
		for (i = 0; i < u->image_height; i++){
			for (j = 1; j < (u->image_width - 1); j++){
				u->image_data[i][j] = u_bar->image_data[i][j];
			}
		}
	}
}

/* Division of image where the height is divided into num_procs parts
with different size depending on the remainder image_height % num_procs.*/
int get_my_height(int image_height, int num_procs, int my_rank){
	int even_dist_height = image_height / num_procs;
	int remainder_height = image_height % num_procs;
	int my_height;

	/* Distrubute the remainder among the first
	(image_height % num_procs) processes.*/
	if (my_rank < remainder_height){
		my_height = even_dist_height + 1;
	}

	else {
		my_height = even_dist_height;
	}

	return(my_height);
}

/* Finds start index for process my_rank's portion of image_chars.*/
int get_my_start(int* sendcounts, int num_procs, int my_rank){
	int my_start = 0, i;

	/* my_start for process my_rank is the sum of elements of
	image_chars assigned to all processes with lower rank than my_rank.*/
	for (i = 0; i < num_procs; i++){
		if (i < my_rank){
			my_start += sendcounts[i];
		}
	}

	return(my_start);
}

/* Main function based on skeleton in template that takes command-line
arguments iters, kappa, input_jpeg_filename, output_jpeg_filename, and
calls the above defined functions to execute the image denoising process.*/
int main(int argc, char* argv[]){
	image u, u_bar;									// Type struct image.
	int image_height, image_width, num_comp;
	unsigned char* image_chars;
	unsigned char* my_image_chars;
	int iters = atoi(argv[3]);						// String to int.
	float kappa = atof(argv[4]);					// String to float.
	char* input_jpeg_filename = argv[5];
	char* output_jpeg_filename = argv[6];
	int num_procs, my_height, my_width, my_rank;	// "Parallel variables".

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Loads in JPEG to process 0 and stores it in image_chars. Also
	prints out information about the imported JPEG to the standard output.*/
	if (my_rank == 0){
		import_JPEG_file(input_jpeg_filename, &image_chars,
			&image_height, &image_width, &num_comp);
		printf("W: %d, H: %d, C: %d, Name: %s\n", image_width,
			image_height, num_comp, input_jpeg_filename);
	}

	MPI_Bcast(&image_height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image_width, 1, MPI_INT, 0, MPI_COMM_WORLD);

	my_height = get_my_height(image_height, num_procs, my_rank);
	my_width = image_width;		// Keeping the size in width.

	allocate_image(&u, my_height, my_width);		// Allocate memory.
	allocate_image(&u_bar, my_height, my_width);	// Allocate memory.

	/* Here declaring variables and subsequently doing communication between
	the processes; all prepearing for the MPI_Scatterv() command further down.*/
	int my_sendcount, my_start, i; // Num. of local chars and local start.
	int* sendcounts = (int*)malloc(num_procs * sizeof(int));
	int* start_indices = (int*)malloc(num_procs * sizeof(int));
	my_sendcount = my_height * my_width;
	my_start = 0;	// To get updated values in second for-loop downwards.

	/* Filling sendcounts and making sure each procs has a copy.*/
	for (i = 0; i < num_procs; i++){
		MPI_Gather(&my_sendcount, 1, MPI_INT, sendcounts,
			1, MPI_INT, i, MPI_COMM_WORLD);
	}

	my_start = get_my_start(sendcounts, num_procs, my_rank);

	/* Filling start_indices and making sure each procs has a copy.*/
	for (i = 0; i < num_procs; i++){
		MPI_Gather(&my_start, 1, MPI_INT, start_indices,
			1, MPI_INT, i, MPI_COMM_WORLD);
	}

	my_image_chars = (unsigned char*)malloc(	// Allocates memory needed below.
		my_sendcount * sizeof(unsigned char));

	/* Partitioning (with different size) image_chars between processes.*/
	MPI_Scatterv(image_chars, sendcounts, start_indices,
		MPI_UNSIGNED_CHAR, my_image_chars, my_sendcount,
		MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	convert_jpeg_to_image(my_image_chars, &u);	// Creates image on each procs.
	IDDP(&u, &u_bar, kappa, iters, num_procs, my_rank); // Iso-diff-denois-parall.
	convert_image_to_jpeg(&u_bar, my_image_chars);	// Convert every image to uc.

	/* Combine all regions of image into image_chars stored on process 0.*/
	MPI_Gatherv(my_image_chars, my_sendcount, MPI_UNSIGNED_CHAR, image_chars,
		sendcounts, start_indices, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	/* Process 0 exports JPEG image from image_chars.*/
	if (my_rank == 0){
		export_JPEG_file(output_jpeg_filename, image_chars,
			image_height, image_width, num_comp, 75);
	}

	deallocate_image(&u);			// Freeing memory from image u.
	deallocate_image(&u_bar);		// Freeing memory from image u_bar.

	MPI_Finalize();

	return(0);
}
