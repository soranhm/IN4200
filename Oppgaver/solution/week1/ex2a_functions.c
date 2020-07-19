#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ex2a_common.h"
#include "minmax.h"

double average(double *arr, int n) {

    double sum = 0;

    for (int i =0; i < n; i ++) sum += arr[i];

    return sum / (double)n;
}

double stdev(double *arr, int n, double average) {

    double dev = 0;
    double term;

    for (int i = 0; i < n; i ++) {
        term = arr[i] - average;
        dev += term * term;
    }

    dev = sqrt(dev/(double)(n - 1));

    return dev;
}

int findminidx(double *arr, int n) {

    int minidx;
    double min;

    minidx = 0;
    min = arr[0];
    for (int i = 0; i < n; i ++) {
        minidx = MINIDX(minidx, i, arr);
    }

    return minidx;
}

int findmaxidx(double *arr, int n) {

    int maxidx;
    double max;

    maxidx = 0;
    max = arr[0];
    for (int i = 0; i < n; i ++) {
        maxidx = MAXIDX(maxidx, i, arr);
    }

    return maxidx;
}


void readfile(char *filename, temperature_data *data) {

    int err;
    FILE *datafile;

    datafile = fopen(filename, "r");

    if (datafile == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }

    err = fscanf(datafile, "%d", &data->n);

    data->times = (char *)malloc(5 * data->n * sizeof(char));
    data->temps = (double *)malloc(data->n * sizeof(double));

    for (int i = 0; i < data->n; i ++) {

        err = fscanf(datafile, "%5s %lf", &data->times[5 * i], &data->temps[i]);

    }

    fclose(datafile);
}
