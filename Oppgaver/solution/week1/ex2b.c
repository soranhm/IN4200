/*
We will see more of this function. It's a nice function.
*/
void smooth(double **v, double **v_new, double c, int m, int n) {

    for (int i = 1; i < n - 1; i ++) {
        for (int j = 1; j < n - 1; j ++) {

            v_new[i][j] = v[i][j] + c * (v[i - 1][j] + v[i + 1][j]  + v[i][j - 1] + v[i][j + 1] - 4 * v[i][j]);

        }
    }
}