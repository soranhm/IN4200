## Exercise 1) Code Balance
We define code balance as the data traffic in words divided by the number of floating point operations.

$$B_c = \frac{\text{Words}}{\text{Flops}}$$


### a) Dense Matrix-Vector Multiply
```C
for (j=0; j<N; j++)
    for (i=0; i<N; i++)
        y[j] += A[j][i]*B[i];
```
For each iteration we have two floating point operations. There are a total of $N^2$ iterations for a total of $2N^2$ flop. Depending on the size of $N$ and the cache size we have two scenarios. Assuming that $N$ is large and that `B` does not fit in cache we end up loading the elements in `B` every single iteration of the `j` loop, $N$ elements loaded $N$ times. `A` is accessed without strides and therefore is loaded once, $N^2$ elements. `y[j]` Can stay in the register for the entire inner loop and is negligible (for large $N$).

$$B_c = \frac{2N^2}{2N^2} = 1$$


### b) Vector Norm
```C
double s = 0.;
for (i=0; i<N; i++)
    s += A[i]*A[i];
```
`s` stays in a register for the entire loop and is negligible. We load `A` without strides which gives a total of $N$ loads. For every iteration there are two flop though.

$$B_c = \frac{N}{2N} = \frac{1}{2}$$


### c) Scalar Product
```C
double s = 0.;
for (i=0; i<N; i++)
    s += A[i]*B[i];
```
Same as  **b)** but we need to load `B` as well.

$$B_c = \frac{2N}{2N} = 1$$


### d) Scalar Product with Indirect Access
```C
double s = 0.;
for (i=0; i<N; i++)
    s += A[i]*B[K[i]];
```
This one is a bit tricky. To constrain the problem a bit I'm going to say that a double is 8 bytes and that an int is 4 bytes. Just counting the loads, without thinking about cache lines. We have two loads for `A` and `B`, and half a load for `K` (size of int is 1/2 that of a double). This gives us a code balance of

$$B_c = \frac{2.5}{2} = 1.25$$

This balance turns out to be true for the case when the integers in `K` are all sequential, $0, 1, \dots, N-1$. But for an array of random integers this is not likely. To illustrate the differences in code balance we can end up with we will look at two more cases.

`K[i]` Constant:
If all the elements in `K` are the same element (or all point to the same cache line). Then we get the best case scenario, We still have $N$ loads for `A` and $N/2$ loads for `K`, but the loads for `B` are negligible.

$$B_c = \frac{N + N/2}{2} = 0.75$$

This is the best case scenario with fewest loads.

`K[i]` Random, always in strides of more than $L_C$:
If we again assume that `B` does not fit in cache, and that `K[i]` is strided in a "maximally bad" way. Then we can imagine that each time we load an elements of `B` we must load the entire cache line that element is on. That is 8 loads for each element in `B`. Adding this to the one load for `A` and half a load for `K` we get

$$B_c = \frac{N + N/2 + 8N}{2} = 4.75$$
