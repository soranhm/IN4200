#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CLOCKS_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

/*
When making a "function" like this via define, wherever it is invoked the
compiler simply replaces the function call with the code. That's why the m,n and
o arguments aren't necessary, because they exist wherever idx is invoked.
Since a conventional function call is slightly time-consuming, defining the
index in this way should be somewhat faster than making a proper index function,
at least when it's called over and over like here.
*/
#define idx(i, j, k) n*o*i + o*j + k

/*
So this is the program I'm guessing caused the most headaches.

First, a few words on the mathematics. As written, the problem is
underdetermined, i.e. it's akin to solving one equation in three unknowns.
There's just way too much freedom to determine how to proceed. I will add the
additional constraints that we know how u(x, y, z, 0) looks at time t = 0
(an initial value); additionally I insist that for t > 0, u is constant on the
boundaries of the cube.

Briefly, it's really just the smooth function again. The heat equation arises in
numerous physical, chemical and even (apparently) financial settings; it's a
simple model describing how "stuff" (e.g. heat) travels from areas of high
concentration to areas of low. Which makes sense!

That's all way beyond the scope of this course. For our purposes, as we will see
later, it has the potential to do nice things to graphics or sound, however.

Second, this marks the return of the dreaded contigious 3D array. While
understanding its structure is, you know, not a bad thing, please for the love
of god do not get hung up on it. I reiterate that the entire reason it is used
is because C just doesn't do 2d/3d arrays like Java or Python do. We use an
underlying 1D array that contains all the cells, and we use two layers of
pointers to move us to the appropriate index in the underlying array. This lets
us access the 3D array as though it were a proper 3D array, but as we've seen
it requires tedious initialisation.

From now on I will drop the pointer layers and use the 1D array directly. It's
1just as well really.

The update rule reads:
u[i][j][k] = u_prev[i][j][k] + c*(u_prev[i][j][k-1] + u_prev[i][j][k+1] +
                                  u_prev[i][j-1][k] + u_prev[i][j+1][k] +
                                  u_prev[i-1][j][k] + u_prev[i+1][j][k] -
                                  6*u_prev[i][j][k])

Each update advances t by dt, and we stop when t = T.

where c is a positive number less than or equal to 1/6 (otherwise the entire
thing goes haywire).

Heads-up: with m = n = o = 100, which is a very coarse discretisation indeed,
there's a million cells. The complexity of this solver grows pretty damn
quickly.

For exercise 4, we just compute. Updatnig a cell is 9 floating point operations.
There are (m-2)*(n-2)*(o-2) cells per time step, and there are N time steps,
for a total of N*9*(m-2)*(n-2)*(o-2) floating point operarions.

Oh yeah! This is an excellent candidate for compiler optimisation!
*/

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
