// Exploring scheduling with openMP

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


int main(int argc, char const *argv[]) {
    int n = 10000000;
    int chunk_size = 10;

    printf("chunk_size = %d\n", chunk_size);

    double *a = malloc(n * sizeof *a);
    double *b = malloc(n * sizeof *b);

    double factor = 1./n;
    for (size_t i = 0; i < n; i++) {
        a[i] = i*factor;
        b[i] = i*factor;
    }

    double dotp = 0;

    // Without openMP:
    double start = omp_get_wtime();
    for (size_t i = 0; i < n; i++)
        dotp += a[i] * b[i];
    double end = omp_get_wtime();
    double tot_s = end - start; // Total serial time.
    printf("Time no omp:  %lf\n", tot_s);

    // Using the default scheduler:
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+ : dotp)
    for (size_t i = 0; i < n; i++)
        dotp += a[i] * b[i];
    end = omp_get_wtime();
    double tot = end - start;
    printf("Time default: %lf, ", tot);
    printf("Speedup: %.2lf\n", tot_s/tot);

    // Using static scheduler:
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+ : dotp) schedule(static)
    for (size_t i = 0; i < n; i++)
        dotp += a[i] * b[i];
    end = omp_get_wtime();
    tot = end - start;
    printf("Time static:  %lf, ", tot);
    printf("Speedup: %.2lf\n", tot_s/tot);

    // Using guided scheduler:
    start = omp_get_wtime();
    // You can add chunk size to the guided schedule, it acts as a minimum.
    #pragma omp parallel for reduction(+ : dotp) schedule(guided)
    for (size_t i = 0; i < n; i++)
        dotp += a[i] * b[i];
    end = omp_get_wtime();
    tot = end - start;
    printf("Time guided:  %lf, ", end - start);
    printf("Speedup: %.2lf\n", tot_s/tot);


    // Using auto scheduler:
    start = omp_get_wtime();
    #pragma omp parallel for reduction(+ : dotp) schedule(auto)
    for (size_t i = 0; i < n; i++)
        dotp += a[i] * b[i];
    end = omp_get_wtime();
    tot = end - start;
    printf("Time auto:    %lf, ", end - start);
    printf("Speedup: %.2lf\n", tot_s/tot);

    free(a);
    free(b);

    return 0;
}
