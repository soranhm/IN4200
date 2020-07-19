#include <stdio.h>
#include <omp.h>
int main (int nargs, char **args) {
#pragma omp parallel
{
  printf("Hello world!\n");
}
return 0;
}
