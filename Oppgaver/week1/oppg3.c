/* Modifing this
void swap(int *a, int *b){
  int t=*a; *a=*b; *b=t;
}
void sort(int arr[], int beg, int end)
{
  if (end > beg + 1)
  {
    int piv = arr[beg], l = beg + 1, r = end;
    while (l < r)
    {
      if (arr[l] <= piv)
        l++;
      else
        swap(&arr[l], &arr[--r]);
    }
    swap(&arr[--l], &arr[beg]);
    sort(arr, beg, l);
    sort(arr, r, end);
  }
}
*/
void swap(int *a, int *b) {
    int t=*a; *a=*b; *b=t;
}
void sort_perm(int *arr, int *perm, int beg, int end) {
    if (end > beg + 1) {

        int piv = arr[perm[beg]], l = beg + 1, r = end;
        while (l < r) {
            if (arr[perm[l]] <= piv)
              l++;
            else
                swap(&perm[l], &perm[--r]);
        }
        swap(&perm[--l], &perm[beg]);
        sort_perm(arr, perm, beg, l);
        sort_perm(arr, perm, r, end);
    }
}
int main(void) {

    int unsorted[20];
    int perm[20];

    for (int i = 0; i < 20; i ++) {
        unsorted[19 - i] = i;
        perm[i] = i;
    }

    sort_perm(unsorted, perm, 0, 20);

    for (int i = 0; i < 20; i ++) printf("%d\n", unsorted[perm[i]]);


}
