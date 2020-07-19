#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "minmax.h"

int main(int narg, char ** args) {
  if (narg < 2) { // amount of arguments
    printf("Input argument required. To run:\n%s n\nwhere n is a positive number.\n",args[0]);
    return EXIT_FAILURE;
  }
  srand(time(NULL));
  int n = atoi(args[1]); //atoi makes string into int
  int *rand_array, min, max;
  if (n <= 0) { // choosen number must be positive
    printf("Input integer must be a positive integer.\n");
    return EXIT_FAILURE;
  }

  // Step 1 : Create the array and fill it with random numbers.
  rand_array = (int *)malloc(n * sizeof(int));

  for (int i = 0; i < n; i++){
    rand_array[i] = rand();
  }

  // Step 2: Obtain min and max values.
  min = max = rand_array[0];

  for (int i = 1; i < n; i ++) {
    min = MIN(min, rand_array[i]);
    max = MAX(max, rand_array[i]);
  }

  printf("Minimum: %d.\nMaximum: %d.\n", min, max);

  // Final step: deallocate the array! Don't forget this part.

  free(rand_array);

}
