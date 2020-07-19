#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ex2a_common.h"

/*
This program has been expanded somewhat to resemble a proper, "big" program.

This seems as good a place as any to introduce structs. A struct is simply a collection
of variables bunched together, so it's not a terribly complicated idea.

It gets a bit messier when we get to structs and struct pointers. Recall that, in Java,
as has hopefully been repeated ad nauseum, whenever you make a declaration like, say,

Scanner thingie;

then you haven't made a Scanner object, but rather a pointer; only when you type

thingie = new Scanner(dot dot dot);

do you make an object. In C, you never make a pointer unless you explicitly
declare it. So upon writing

temperature_data data;

you do make a temperature_data struct. One can then access the struct's variables in familiar fashion:

data.n = 10;

This is, of course, fine for small programs.
If we were to use pointers instead, it would look like this:

temperature_data *data = (temperature_data *)malloc(sizeof(temperature_data));

Data is accessed via ->, e.g.

data->n = 10;

I personally prefer this approach, especially for larger programs. Just remember to use free at the end.
*/

int main(int narg, char **argv) {

    temperature_data *data;
    int minidx, maxidx;
    double avg, dev;

    if (narg < 2) {

        printf("Filename required.\n");
        exit(0);
    }

    data = (temperature_data *)malloc(sizeof(temperature_data));

    readfile(argv[1], data);

    minidx = findminidx(data->temps, data->n);
    maxidx = findmaxidx(data->temps, data->n);
    avg = average(data->temps, data->n);
    dev = stdev(data->temps, data->n, avg);

    for (int i = 0; i < data->n; i ++) {
        printf("%.5s %lf\n", &data->times[5 * i], data->temps[i]);
    }

    printf("Lowest temperature  %f at time %.5s\n", data->temps[minidx], &data->times[5 * minidx]);
    printf("Highest temperature  %f at time %.5s\n", data->temps[maxidx], &data->times[5 * maxidx]);
    printf("Mean temperature %f, with standard deviation %f.\n", avg, dev);

    free(data->times);
    free(data->temps);
    free(data);

}
