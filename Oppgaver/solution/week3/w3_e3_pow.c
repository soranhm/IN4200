// Solution to week 3 ex. 3, Custom pow function.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

double pow100(double x);

int main(int argc, char const *argv[])
{
    int n = 10000;
    double random_value, *x = malloc(n * sizeof *x);
    double *temp = malloc(n * sizeof *temp);

    for (size_t i = 0; i < n; i++) {
        //float in range -1 to 1
        random_value = (double)rand()/RAND_MAX*2.0-1.0;
        x[i] = random_value;
    }

    // Verification of pow100().
    printf("pow(1.5, 100)    = %e\n", pow(1.5, 100));
    printf("pow100(1.5, 100) = %e\n", pow100(1.5));

    // Regular pow
    clock_t start_pow = clock();
    for (size_t i = 0; i < n; i++) {
        temp[i] = pow(x[i], 100);
    }
    clock_t end_pow = clock();
    double tot_pow = (double)(end_pow - start_pow)/CLOCKS_PER_SEC;

    // pow100
    clock_t start = clock();
    for (size_t i = 0; i < n; i++) {
        temp[i] = pow100(x[i]);
    }
    clock_t end = clock();
    double tot = (double)(end - start)/CLOCKS_PER_SEC;

    printf("Time regular pow: %lfms\n", tot_pow*1000);
    printf("Time pow100:      %lfms\n", tot*1000);
    printf("Speedup: %lf\n", tot_pow/tot);

    free(x);
    free(temp);

    return 0;
}

double pow100(double x)
{
    double x4, x32, xp;
    xp = x*x; // x^2
    xp *= xp;  // x^4
    x4 = xp;
    xp *= xp; // x^8
    xp *= xp; // x^16
    xp *= xp; // x^32
    x32 = xp;
    xp *= xp; // x^64

    return (x4*x32*xp); //x^(4+32+64)
}
