# README Week 4 of IN3200/IN4200


## Header-only libraries
It is often useful to separate out code that is used by several different programs in to its own header file. This way you can include the header in all the programs that rely on that code and reuse without having to copy/paste.

This week we will create a (small) "header-only" library. Header-only just means that all the code and definitions are all in header files. Header files usually have the suffix `.h`. For bigger projects it might be better to make proper packages, but for now we will keep it simple.

A good practice in header files is to use a header guard. The header guards function is to avoid including the same header several times. To avoid this we use a preprocessor directive `#ifndef`.  

```C
#ifndef HEADER_FILE_H
#define HEADER_FILE_H

// Your code here.

#endif
```

If `HEADER_FILE_H` is previously defined, everything between `#ifndef` and `#endif` is skipped. Meaning that if the header is included several times, the code is only included once.

To include your header file we use `#include`
```C
#include "header_file.h"
```
Notice that we replaced `<>` with `""`.

If you want to see what the preprocessor actually does to your program you can use compiler flags to get the output from the preprocessor. If you are using `gcc` the `-E` option does this.
```bash
$ gcc my_source_code.c -o after_prep.c -E
```

## Week 4 Header File
The exercises this week all share a header file. It's strictly not needed for such small programs, but illustrate how to make header only libraries.

## Makefile
Only one minor addition to the makefile this week. Since our programs now depend on two files, a `.c` and `.h` file we must tell make of that dependence.
```make
DEP = week4.h

target.exe : source.c $(DEP)
  $(CC) $< -o $@
```
Now `target.exe` will be recompiled whenever `source.c` or `week4.h` change.

## Exercise 1) Memory Bandwidth
This exercise is one of the STREAM micro benchmarks. The program is quite simple and the performance is much better when compiled with high optimization level. Using `-O3` can give a speedup close to 3X. Use `-fopt-info` if you are using the `gcc` compiler and want to see what kind of optimization steps the compiler did.

## Exercise 2) Strided Access
Each time a value is fetched from main memory to cache, an entire cache line is fetched. If we access all the elements in an array one after the other then the cache hit rate is maximal. Every value fetched is used. If we introduce a stride, some of the values we fetched will not be used. And we will get a cache miss much more often.

If we have a cache line that can hold $N$ words/doubles, and the stride we use is $s$. Then the average cache utilization is given by

$$\frac{\text{\# words used on average}}{\text{\# words in cache line}} = \frac{\frac{N}{s}}{N} = \frac{1}{s} $$

This gives us the following table, note the last row assumes a cache line length of 8 words. When stride is greater than the word length of the cache line only one element is used in every line.

| Stride | Utilization |
|-------:|------------:|
|    1   | 100%        |
|    2   | 50%         |
|    3   | 33%         |
|    4   | 25%         |
|    5   | 20%         |
|    6   | 17%         |
|    7   | 14%         |
|$\geq 8$| 12%         |

Since the STREAM benchmark we are doing is memory bound we should see a severe performance hit due to the frequent cache misses when stride increases.

## Exercise 3) Matrix-Matrix Multiplication
Matrix-matrix multiplication is a classic  problem in HPC. To get an overview of some of the algorithms see [Wikipedia](https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm).

### a)
If we have two matrices $A\sim n\times m$, and $B\sim m\times p$ then the matrix $C = AB\sim n\times p$ is defined by (assuming zero based indexing)

$$c_{ij} = \sum_{k=0}^{m-1}a_{ik}b_{kj}$$

First we need to [allocate 2D arrays](https://www.uio.no/studier/emner/matnat/ifi/IN3200/v19/teaching-material/multidimarrays.pdf). Then we must implement the algorithm. The simplest approach is a triple sum over $i, j$ and $k$. The downside to this approach is that we end up loading the same cache lines over and over, but more on that later.

```
for i from 0 to n-1:
    for j from 0 to p-1:
        sum <- 0;
        for k from 0 to m-1:
            sum <- sum + A[i,k]*B[k,j];
        C[i,j] <- sum:
```

### b)
Depending on the size of the matrices different number of loads/stores will be done. We denote $M$ as the cache size and $L_C$ as the length of a cache line, both in number of words (doubles).

If the matrices are so small that all the matrices fit in the cache, $n\times m + m\times p +n\times p< M$. The first few accesses to elements in B will incur a cache miss, but the lines will probably stay in the cache for the next iterations. We end up with close to

$$\frac{n\times m + m\times p}{L_C}, \hspace{.5cm} \text{Loads} \\
  \frac{n\times p}{L_C}, \hspace{.5cm} \text{Stores} \\
$$

If the matrices are larger we are in a different situation. We access the memory in $A$ without strides, so the number of cache line loads are $n\times m/L_C$, and $n\times p/L_C$ writes to $C$. For $B$ it's a different matter. When one row of $B$ has more elements than fits in a cache line then $m\times L_C$ words of the cache is used to store one column of $B$, and when $n\times L_C > M$ we incur a cache miss on every element of $B$, every time. Since each element of $B$ is used in $n$ computations, and there are $m\times p$ elements in $B$ we get $n\times m\times p$ cache line loads! The total number of loads and stores are

$$\frac{n\times m}{L_C} + n\times m\times p, \hspace{.5cm} \text{Loads} \\
\frac{n\times p}{L_C}, \hspace{.5cm} \text{Stores}$$

### c)
To improve performance we must figure out what the bottleneck is. From **b)** we know that we must load an entire cache line for every element in B, every time it is used (assuming "large" matrix). Increasing the cache hits when accessing elements in B is therefore a worthwhile strategy.

If element `B[k][j]` is at the start of a cache line then `B[k][j+1]`, `B[k][j+2]`, `B[k][j+3]` ... `B[k][j+Lc-1]` will be loaded with that element. To increase the cache hits we must use these values before they are flushed.

The implementation in the source code unrolls 4 times and should show a significant performance increase.

**Pseudocode:**
```
rest <- p mod 4;
for i from 0 to n-1:
    for j from 0 to p-rest-1, 4:
        for k from 0 to m-1:
            C[i,j]   <- C[i,j]   + A[i,k]*B[k,j];
            C[i,j+1] <- C[i,j+1] + A[i,k]*B[k,j+1];
            C[i,j+2] <- C[i,j+2] + A[i,k]*B[k,j+2];
            C[i,j+3] <- C[i,j+3] + A[i,k]*B[k,j+3];
    // The rest:
    for j from p-rest to p-1:
        for k from 0 to m-1:
            C[i,j]   <- C[i,j]   + A[i,k]*B[k,j];
```

**Note:** We would get maximum cache hits if we unroll the loop with the length of the cache line, but that would require to hold $2L_C+1$ values in registers, an Intel i7 has 16 64 bit (double) registers, and $L_C = 8$. If we try to unroll the loop to use the full cache line then we would need $17$ registers. But we only have $16$ available. The program will deal with this by moving values in and out or registers. But at the point where we are using 16 registers we will not see the same performance increase. Different CPUs will have a different amount of registers.
