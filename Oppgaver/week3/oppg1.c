#include <stdlib.h>
#include <math.h> // M_PI
#include <stdio.h>
#include <time.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

double numerical_integration (double x_min, double x_max, int slices);

int main(int argc, char const *argv[]) {
  int n_div;
  if (argc > 1) {
    n_div = atoi(argv[1]);
  }
  else {
    n_div = 10000;
  }


  int n = 3;
  int* slices = malloc(n*sizeof*slices);
  slices[0] = 10;

  for (size_t i = 1; i < n; i++){
    slices[i] = slices[i-1]*10;
  }
  double value, rel_e;
  for (size_t i = 0; i<n;i++){
    value = numerical_integration(0,1,slices[i]);
    rel_e = value/M_PI -1;
    printf("slices: %*d, integral: %lf, relative error: %lf\n",
           n+1, slices[i], value, rel_e);
  }


  unsigned long long start = __rdtsc();
  value = numerical_integration(0,1,n_div);
  unsigned long long end = __rdtsc();

  double avg = (double)(end -start)/n_div;
  printf("Average number of cycles: %lf\n", avg);
  free(slices);

  return 0;

}
double numerical_integration (double x_min, double x_max, int slices)
{
  double delta_x = (x_max-x_min)/slices;
  double x, sum = 0.0;

  for (int i=0; i<slices; i++) {

    x = x_min + (i+0.5)*delta_x;
    sum = sum + 4.0/(1.0+x*x);
  }
  return sum*delta_x;
}
