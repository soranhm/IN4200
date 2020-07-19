#ifndef ODDEVEN_H
#define ODDEVEN_H

typedef int numeric;

int compare_exchange(numeric *a, numeric *b){
    numeric tmp;
    if (*a > *b) {
        tmp = *a;
        *a = *b;
        *b = tmp;
        return 1;
    }
    return 0;
}


int oddeven(numeric *a, int n){
    for (size_t i = 0; i < n; i++) {
        if (i%2 == 1) {
            for (size_t j = 0; j < n/2; j++) {
                compare_exchange(&a[2*j], &a[2*j+1]);
            }
        } else {
            for (size_t j = 0; j < n/2 - 1; j++) {
                compare_exchange(&a[2*j+1], &a[2*j+2]);
            }
        }
    }
    return 0;
}


int oddeven_anylenght(numeric *a, int n){
    int r = n%2 - 1; // Used in the even step.
    for (size_t i = 0; i < n; i++) {
        if (i%2 == 1) {
            for (size_t j = 0; j < n/2; j++) {
                compare_exchange(&a[2*j], &a[2*j+1]);
            }
        } else {
            for (size_t j = 0; j < n/2 + r; j++) {
                compare_exchange(&a[2*j+1], &a[2*j+2]);
            }
        }
    }
    return 0;
}


int oddeven_stop(numeric *a, int n){
    int r = n%2 - 1;
    int changed_odd = 0;
    int changed_even = 0;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n/2; j++) {
            if (compare_exchange(&a[2*j], &a[2*j]+1)) {
                changed_odd = 1;
            }
        }
        for (size_t j = 0; j < n/2 + r; j++) {
            if (compare_exchange(&a[2*j+1], &a[2*j+2])) {
                changed_even = 1;
            }
        }
        if (!changed_odd && !changed_even) {
            return i+1;
        }
        changed_odd = changed_even = 0;
    }
    return n;
}


int oddeven_omp(numeric *a, int n){
    int r = n%2 - 1;
    int changed_odd = 0;
    int changed_even = 0;
    #pragma omp parallel
    {
        for (size_t i = 0; i < n; i++) {
            #pragma omp for reduction(+:changed_odd)
            for (size_t j = 0; j < n/2; j++) {
                if (compare_exchange(&a[2*j], &a[2*j]+1)) {
                    changed_odd = 1;
                }
            }
            // Implicit barrier here.
            #pragma omp for reduction(+:changed_even)
            for (size_t j = 0; j < n/2 + r; j++) {
                if (compare_exchange(&a[2*j+1], &a[2*j+2])) {
                    changed_even = 1;
                }
            }

            if (!changed_odd && !changed_even) {
                break;
            }
            #pragma omp barrier
            #pragma omp single
            {
                changed_odd = changed_even = 0;
            }
        }
    }
    return 0;
}


int printvec(numeric *a, int n){
    printf("[%d,", a[0]);
    for (size_t i = 1; i < n-1; i++) {
        printf(" %d,", a[i]);
    }
    printf(" %d]\n", a[n-1]);
    return 0;
}


#endif
