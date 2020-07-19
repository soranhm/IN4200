typedef struct {

    int n;
    char *times;
    double *temps;
}temperature_data;

double average(double *arr, int n);
double stdev(double *arr, int n, double average);
int findminidx(double *arr, int n);
int findmaxidx(double *arr, int n);
void readfile(char *filename, temperature_data *data);
