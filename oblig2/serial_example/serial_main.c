#include <stdio.h>

#ifdef __MACH__
#include <stdlib.h>
#else 
#include <malloc.h>
#endif

/* The purpose of this program is to demonstrate how the functions
   'import_JPEG_file' & 'export_JPEG_file' can be used. */

void import_JPEG_file (const char* filename, unsigned char** image_chars,
                       int* image_height, int* image_width,
                       int* num_components);
void export_JPEG_file (const char* filename, const unsigned char* image_chars,
                       int image_height, int image_width,
                       int num_components, int quality);


int main (int argc, char *argv[])
{
  int height, width, comp, i,j,k;
  unsigned char *image_chars, *new_chars;
  
  import_JPEG_file("toys.jpg", &image_chars, &height, &width, &comp);
  printf("Succeeded! vertical pixels: %d, horizontal pixels: %d, num components: %d\n",
	 height, width, comp);

  /* creating a horizontally flipped image */
  new_chars = (unsigned char*)malloc(height*width*comp*sizeof(unsigned char));
  for (i=0; i<height; i++)
    for (j=0; j<width; j++)
      for (k=0; k<comp; k++)
	new_chars[i*(width*comp)+j*comp+k]=image_chars[i*(width*comp)+(width-j-1)*comp+k];

  export_JPEG_file ("flipped_image.jpg", new_chars, height, width, comp, 75);
  return 0;
}
