#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
Note:
This program isn't strictly part of the exercises, but it shows two things: one, how to make a
contigious 2d array, which I hope helps in understanding the much more difficult task
of creating a contigious 3d array (ex1d); second, it demonstrates that it indeed matters
whether one traverses the resulting matrix rowwise or columnwise.

Finally, since the program is timed, I thought this would be a good place to show off
the compiler's optimisation feature. Compile with

> make ex1c_optimised

then run the resulting ex1c_optimised to see how the compiler can rewrite your
code for a bit of speedup.
-------------------------------------------------------------------------------------------------------------------------------------------
Purpose of program: test whether traversing a 2d array column by column is as
fast as traversing row by row.

The answer, by the way, should be no. Can you think why?
*/
int main(void) {

    int m, n;
    double **A;
    clock_t start, timer_rowwise, timer_colwise;

    m = n = 10000;  // I'm picking m and n very, very big just to make sure the effect is noticeable.

    A = (double **)malloc(m * sizeof(double *));
    A[0] = (double *)malloc(m * n * sizeof(double)); // This is the actual underlying 1d array of size m * n.

    for (int i = 1; i < m; i ++) {
        A[i] = &(A[0][n * i]);  // This line probably requires some unpacking. Just note that A[0][i * n] is the i'th row of the matrix A,
                                        // and that's what we want A[i] to point to.
        //A[i] = &(A[i-1][n]) // This is equivalent but has recursion. Hooray for recursion!
        //A[i] = A[i -1] + n; // Also equivalent to the above, but uses so-called pointer arithmetic which I just don't like personally.
    }

    // First test: Assign values in the usual order.
    start = clock();

    for (int i = 0; i < m; i ++) {
        for (int j = 0; j < n; j ++) {
            A[i][j] = m * i + j;
        }
    }

    timer_rowwise = clock() - start;

    // Second test: Assign values in the reverse order.
    start = clock();

    for (int j = 0; j < n; j ++) {
        for (int i = 0; i < m; i ++) {
            A[i][j] = m * i + j;
        }
    }

    timer_colwise = clock() - start;

    // Result
    printf("Time elapsed using i-j-loop: %lu millisec.\n", 1000*timer_rowwise/CLOCKS_PER_SEC);
    printf("Time elapsed using j-i-loop: %lu millisec.\n", 1000*timer_colwise/CLOCKS_PER_SEC);

    free(A[0]);
    free(A);

}