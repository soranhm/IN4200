#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(void) {

    int num_iter, n;

    num_iter = 20;
    n = 1;

    double tolerance, limit, current, add;

    limit = 4./5.;
    tolerance = 1e-12;
    current = add = 1.;

    printf("Verifying that the series 1 - 2^(-2) + 2^(-4) - 2^(-6) + ... converges to %1.1f,\n" \
           "summing up to %d terms and checking against a tolerance of %.1e.\n\n", limit, num_iter, tolerance);

    while (n ++ < num_iter && fabs(current - limit) > tolerance) {

        add *= -.25;
        current += add;
    }

    if (fabs(current - limit) > tolerance) {

        printf("The series failed to converge within tolerance. Expected %f, vs. actual %f. \n", limit, current);
        return EXIT_FAILURE;
    } else {
        printf("The series converged as expected.  Number of terms required before sum within tolerance: %d. \n", n);
    }

    return 0;
}