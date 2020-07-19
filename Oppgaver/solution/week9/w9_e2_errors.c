
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


int main(int argc, char const *argv[]) {

    int i, N=100000;
    double *u = malloc(N * sizeof *u);
    double *v = malloc(N * sizeof *v);

    for (i=0; i<N; i++) {
        u[i] = 0.001*(i - N/2);
    }
    v[0] = u[0];
    v[N-1] = u[N-1];

    #pragma omp parallel default(shared) // default(shared) does nothing.
    {
        int time_step;
        double *tmp;
        for (time_step=0; time_step<1000; time_step++)
        {
            #pragma omp for
            for (i=1; i<N-1; i++)
                v[i] = u[i-1] - 2*u[i] + u[i+1];

            #pragma omp single
            {
                tmp = v;
                v=u;
                u=tmp;
            }
        }
    }

    return 0;
}
