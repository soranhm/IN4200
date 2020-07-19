// Implementation of heap_select, and structure definition example.

#ifndef WEEK9_H
#define WEEK9_H

typedef struct graph{
    int n_nodes;
    int n_edges;
    int *in;
    int *out;
    int *self;
} webGraph;

/**
Return the indexes of the k largest elements in arr.

Parameters
----------
    arr : Array with the values to select from.

    n : Lenght of arr

    k : Lenght of idx.

Return Parameters
-----------------
    idx : Array that will contain the indices of the top k entries in arr.

Notes
-----
Assumes n > k

This is a selection algorithm called heap select. The elements are not sorted
when the function returns.
*/
int* heap_select(int* arr, int n, int* idx, int k){
    int idxmin = 0; // Minimum of the k largest values.
    int heapmin = arr[0];

    // Setting the elements in idx to the first k values in arr.
    for (size_t i = 0; i < k; i++) {
        idx[i] = i;
        if (heapmin > arr[i]) {
            heapmin = arr[i];
            idxmin = i;
        }
    }

    for (size_t i = k; i < n; i++) {
        if (heapmin < arr[i]) {
            idx[idxmin] = i; // Replace the minumim value in idx min.
            heapmin = arr[i];
            for (size_t j = 0; j < k; j++) {
                if (heapmin > arr[idx[j]]) {
                    heapmin = arr[idx[j]];
                    idxmin = j;
                }
            }
        }
    }

    return 0;
}


#endif
