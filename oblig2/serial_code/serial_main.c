#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <time.h>
#include "../simple-jpeg/import_export_jpeg.h"

#define BILLION 1E9

/* needed header files .... */
/* declarations of functions import_JPEG_file and export_JPEG_file */

int main(int argc, char *argv[]){
  int m, n, c, iters;
  float kappa;
  image u, u_bar;
  unsigned char *image_chars;
  char *input_jpeg_filename,*output_jpeg_filename;

  struct timespec tick, tock;

  /* read from command line: kappa, iters, input_jpeg_filename, output_jpeg_filename */
  if (argc < 5)
  {
      printf("\033[1;31mRUN:\n./serial_main.x kappa iters input_file output_file\033[0m\n");
  }
  sscanf(argv[1], "%f", &kappa);
  sscanf(argv[2], "%d", &iters);
  input_jpeg_filename = argv[3];
  output_jpeg_filename = argv[4];

  printf("\033[1;32mIMPORTING JPEG FILE ...\033[0m\n");
  import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
  printf("\033[1;32mm = %d, n = %d \033[0m\n",m,n);
  printf("\033[1;32mWith %d iterations and kappa = %.2f\033[0m\n",iters,kappa );
  printf("\033[1;32mALLOCATING IMAGE ...\033[0m\n");
  allocate_image(&u, m, n);
  allocate_image(&u_bar, m, n);

  printf("\033[1;32mCONVERGIN JPEG TO IMAGE\033[0m\n");
  convert_jpeg_to_image(image_chars, &u);
  printf("\033[1;32mISO DIFFUSION DENOISING\033[0m\n");

  /* measure monotonic time */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tick);	/* mark start time */
  iso_diffusion_denoising(&u, &u_bar, kappa, iters);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tock);	/* mark the end time */

  double execTime = (1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec)/BILLION;

  printf("\033[1;32mCONVERTING IMAGE TO JPEG ...\033[0m\n");
  convert_image_to_jpeg(&u_bar, image_chars);

  printf("\033[1;32mEXPORTING JPEG FILE ...\033[0m\n");
  export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);

  printf("\033[1;32mDEALLOCATING IMAGE ...\033[0m\n");
  deallocate_image(&u);
  deallocate_image(&u_bar);

  printf("\033[1;31mTIME: %lf s\033[0m\n",execTime);
  return 0;
}
