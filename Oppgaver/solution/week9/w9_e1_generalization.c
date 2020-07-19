
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Usage: ./w9_e1_generalization.c M P\n");
        return 1;
    }
    int m = atoi(argv[1]);
    int p = atoi(argv[2]);

    int time =  m/(3*p) + (m + 3*p - 1)/(3*p) + ((m%(3*p))>p?1:0);

    printf("With %d tasks, %d workers and %d super workers\n", m, p, p);
    printf("Time to completion is: %d\n", time);
    return 0;
}
