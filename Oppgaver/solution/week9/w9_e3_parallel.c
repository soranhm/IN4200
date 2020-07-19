#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

int main(int argc, char const *argv[]) {

    int i, N = 100000;
    double s = 0.;

    // Serial implementation:
    double *a1 = (double*)malloc (N*sizeof(double));
    double *b1 = (double*)malloc (N*sizeof(double));

    double t_s1 = omp_get_wtime();
    for (i=0; i<N; i++) {
        a1[i] = 10.0+2*i;
        b1[i] = 20.0+sin(0.1*i);
    }

    for (i=0; i<N-1; i++) {
        s += a1[i];
        a1[i+1] = cos(b1[i]);
    }
    double t_e1 = omp_get_wtime();
    double tot1 = t_e1 - t_s1;
    printf("Serial result:   %.3lf, Time = %6.3fms\n", s, 1000*tot1);

    // Serial improved:
    s = 0.;
    double *a2 = (double*)malloc (N*sizeof(double));

    double t_s2 = omp_get_wtime();
    a2[0] = 10.0;
    for (i=0; i<N-1; i++) {
        a2[i+1] = cos(20.0+sin(0.1*i));
        s += a2[i];
    }
    double t_e2 = omp_get_wtime();
    double tot2 = t_e2 - t_s2;

    printf("Improved result: %.3lf, Time = %6.3fms, Speedup = %6.3lf\n",
           s, 1000*tot2, tot1/tot2);


    // parallel implementation:
    s = 0.;
    double *a3 = (double*)malloc (N*sizeof(double));

    double t_s3 = omp_get_wtime();
    a3[0] = 10.0;
    s += a3[0];
    #pragma omp parallel for reduction(+:s)
    for (i=1; i<N-1; i++) {
        a3[i] = cos(20.0+sin(0.1*(i-1)));
        s += a3[i];
    }
    a3[N-1] = cos(20.0+sin(0.1*(N-2))); // This is just here to make sure that a3 == a2
    double t_e3 = omp_get_wtime();
    double tot3 = t_e3 - t_s3;
    printf("Parallel result: %.3lf, Time = %6.3fms, Speedup = %6.3lf\n",
           s, 1000*tot3, tot1/tot3);

    free(a1);
    free(b1);
    free(a2);
    free(a3);

    return 0;
}
