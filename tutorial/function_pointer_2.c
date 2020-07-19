#include <stdio.h>

int *getMax(int *m, int *n) {
  /* if the value pointed by pointer m is greater than n
   * then, return the address stored in the pointer variable m */
  if (*m > *n) {
    return m;
  }
  else {
    return n;
  }
}

int main(void) {
  // integer variables
  int x = 100;
  int y = 200;
  // pointer variable
  int *max = NULL;
  /* get the variable address that holds the greater value
   * for this we are passing the address of x and y
   * to the function getMax() */
  max = getMax(&x, &y);
  // print the greater value
  printf("maxvalue is: %d \n ",max);
  return 0;
}

// Should return y = 200 
