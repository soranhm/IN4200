#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CLOCKS_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

#define idx(i, j, k) n*o*i + o*j + k

void solve(double *u, double *u_prev, double c, int m, int n, int o, int num_iter) {

    int counter = 0;
    double *tmp;

    while (counter++ < num_iter) {

        for (int i = 1; i < m - 1; i ++) {
            for (int j = 1; j < n - 1; j ++) {
                for (int k = 1; k < o - 1; k ++) {
                    u[idx(i,j,k)] = u_prev[idx(i,j,k)] + c * (u_prev[idx(i, j, k - 1)] + u_prev[idx(i, j, k + 1)] +
                                                 u_prev[idx(i, j - 1, k)] + u_prev[idx(i, j + 1, k)] +
                                                 u_prev[idx(i - 1, j, k)] + u_prev[idx(i + 1, j, k)] - 6 *u_prev[idx(i, j, k)]);
                }
            }
        }

        // Swap u and u_prev
        tmp = u_prev;
        u_prev = u;
        u = tmp;
    }
}

void set_initial_values(double *u, double *u_prev, int m, int n, int o) {
    double denom = 1./((m-1)*(n-1)*(o-1));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < o; k++) {
                u[idx(i, j, k)] = 2.*sin(i*j*k*M_PI*denom);
            }
        }
    }
    memcpy(u_prev, u, m*n*o*sizeof *u);
}

int main(void) {

    int m, n, o, N; // N is number of time steps.
    double kappa, dt, dx, T, c;  // assume dx = dy = dz.
    double *u, *u_prev;
    clock_t start, total, time_ms;
    unsigned long num_flo;

    kappa = 1;
    dx = .05;
    dt = 1e-4;
    T = 1;

    c = kappa * dt / (dx * dx);
    if (c > 1./6.) printf("Warning: c exceeds stability criterion, c = %lf\n", c);

    // Compute m, n, o, N;
    N = (int)(T / dt);
    T = N * dt;
    m = n = o = (int)(2. / dx);
    num_flo = 9*(m - 2)*(n - 2)*(o - 2)*N;

    // calloc is malloc plus setting everything to zero.
    u = (double *)calloc(m * n * o, sizeof(double));
    u_prev = (double *)calloc(m * n * o, sizeof(double));

    set_initial_values(u, u_prev, m, n, o);
    printf("Initial values set.\n");

    printf("Using a %d x %d x %d grid, and %d time steps.\n", m, n, o, N);
    printf("Performing %lu floating point operations.\n", num_flo);
    start = clock();
    solve(u, u_prev, c, m, n, o, N);
    total = clock() - start;
    time_ms = CLOCKS_TO_MILLISEC(total);
    printf("Completed! Time elapsed %lu ms.\nFlops: %f\n\n", time_ms, (float)num_flo/(float)time_ms*1000.);

    free(u);
    free(u_prev);

    return 0;
}
