#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CLOCKS_TO_MILLISEC(t) (t*1000)/CLOCKS_PER_SEC

/*
There's not a ton to say about this program. Reading and writing with bin-files
has several advantages, speed notwithstanding. The binary data format is
essentially identical to how the data is stored in memory, so it's just a matter
of copying data from one place to another. As well, converting floating point
numbers to text induces some slight roundoff error.

There are some drawbacks, of course, mainly to do with safety. Even simple
mistakes like reading as a float array a file that has been stored as a double
array can potentially render the data unreadable. Worse, it's conceivable that
different computers interpret data differently. And since the data is unreadable
to a human, it's not like you can open the file and check what's appropriate
One compromise is to store the data in binary format, but include an ASCII file
with some basic info.

Anyway, that was a great big old digression. Enjoy the code, in all its
quick-and-dirty glory.
*/

// This function is just here to confuse you.
void initialise(double *arr, int len) {

    for (int i = 0; i < len; i ++) arr[i] = i;
}


int main(int narg, char **argv) {

    clock_t start, ascii_read_timer, ascii_write_timer, bin_read_timer, bin_write_timer;

    int n = 1e5;
    double *data, *fromfile;
    FILE *binfile, *asciifile;

    data = (double *)malloc(n * sizeof(double));
    initialise(data, n);

    // Write to ASCII file
    asciifile = fopen("data.txt", "w");

    start = clock();
    for (int i = 0; i < n; i ++) fprintf(asciifile, "%lf\n", data[i]);
    ascii_write_timer = clock() - start;

    fclose(asciifile);

    // Read from ASCII file
    asciifile = fopen("data.txt", "r");
    fromfile = (double *)malloc(n * sizeof(double));

    start = clock();
    for (int i = 0; i < n; i ++) fscanf(asciifile,"%lf", &fromfile[i]);
    ascii_read_timer = clock() - start;

    fclose(asciifile);
    free(fromfile);

    // Write to bin file
    binfile = fopen("data.bin", "wb");

    start = clock();
    fwrite(data, sizeof(double), n, binfile);
    bin_write_timer = clock() - start;

    fclose(binfile);

    // Read from bin file
    fromfile = (double *)malloc(n * sizeof(double));
    binfile = fopen("data.bin", "rb");

    start = clock();
    fread(fromfile, sizeof(double), n, binfile);
    bin_write_timer = clock() - start;

    fclose(binfile);
    free(fromfile);

    //
    printf("Time elapsed writing to ASCII file: %lu ms.\n", CLOCKS_TO_MILLISEC(ascii_write_timer));
    printf("Time elapsed writing to binary file: %lu ms.\n", CLOCKS_TO_MILLISEC(bin_write_timer));
    printf("Time elapsed reading from ASCII file: %lu ms.\n", CLOCKS_TO_MILLISEC(ascii_read_timer));
    printf("Time elapsed reading from binary file: %lu ms.\n", CLOCKS_TO_MILLISEC(bin_read_timer));

    // And that's it
    free(data);
}
