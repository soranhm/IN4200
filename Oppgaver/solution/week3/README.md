# README week 3 of IN3200/IN4200

To compile the programs for this week:
```
$ make all
```

## Makefile
In the makefile for this week we introduce a couple of new concepts. [Automatic variables](https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html), [wildcards](https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html) and [string substitution](https://www.gnu.org/software/make/manual/html_node/Text-Functions.html).

### Automatic variables
In the makefile for this week we use two automatic variables, `$<` and `$@`. The first one, `$<` is turned in to the name of the first prerequisite. `$@` is turned in to the name of the target. If we have the recipe
```
myprog.exe : myprog.c
  gcc $< -o $@
```
The command make runs is
```
gcc myprog.c -o myprog.exe
```

### Wildcards and string substitution
Wildcards are used to match all files or strings that match a pattern. To create a variable that contain all the files in the local directory ending in `.c` but with the suffix changed to `.exe` you can use
```
PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
```
Notice that there are two different ways to declare a wildcard. In the `wildcard` function `*` is used, and in the `patsubst` function a `%` is used.

`$(wildcard *.c)` is expanded to a whitespace separated list of all filenames ending in `.c`, and `$(patsubst %.c, %.exe, ...` changes the suffix of all of them.

## Exercise 1) Division
To estimate the latency we have to make a few assumptions. **First**: we assume that there is not going to be any latency due to cache misses. The L1 cache of standard laptops is measured in KB. So all the values used in the function is going to fit in the L1 cache. They will also be used over and over, making it unlikely that they get flushed back to main memory. **Second**: the computation inside the loop is dominated by the division. If this is true we can say

\(T = T_{division} + T_{rest} \approx T_{division}\)

**Third**: SIMD instructions are not used. There is not a dependency between each iteration in the loop, you can vectorize this function. To explicitly avoid compiler optimization you can use the `-O0` flag.

If you try to test the execution time of this function, and compare it to your CPU division latency, you might still see that your program runs faster than the stated latency for a divide on your CPU. Can you think of any [reasons](https://en.wikipedia.org/wiki/Out-of-order_execution) for this?  

Depending on how you count the cycles you might get different results. Using `__rdtsc()` as show in the suggested solution has one problem. Try running the program with many slices while your computer does something else, like streaming a youtube video.

**Side note**: The integration method shown in this exercise is called the [midpoint method](https://en.wikipedia.org/wiki/Midpoint_method).

## Exercise 2) Prefetching
```C
double s = 0.;
for (i=0; i<N; i++)
    s = s + A[i]*B[i];
```
I'm going to assume a 1GHz (or higher) clock cycle for this exercise.

### a)
Since a cache line holds consecutive elements in memory one cache line must be fetched for each array. At this point there will be data in the cache to complete four multiplications, and four additions, for a total of 8 flops.

The total number of bytes on one cache line is 32 (4 double precision floats). If we divide by the bandwidth we get

$$\frac{32\text{Bytes}}{3.2\text{GBytes/sec}} = 10\text{ns}$$

The latency to load one cache line is 100ns. The total for both cache lines is then 220ns. Since the flops in this situation is negligible compared to the memory management the performance is approximately \(8\text{Flops}/220\text{ns} = 36\text{MFlops/sec}\)

### b)
From **a)** we know that the time used to load one cache line is 10ns. Using eq. 1.6 from the textbook we then get

$$P = 1 + \frac{100\text{ns}}{10\text{ns}} = 11$$

A total of 11 outstanding prefetches are necessary.

### c)
For line lengths two and four times as long, the transfer time without latency is 20ns and 40ns respectively. Inserting this into eq. 1.6 we get

$$P_2 = 1 + \frac{100\text{ns}}{20\text{ns}} = 6$$

$$P_4 = 1 + \frac{100\text{ns}}{40\text{ns}} = 3.5$$

Of course we can't do half of a prefetch so \(P_4 = 4\).

### d)
Without latency we can load two cache lines in 20ns. With the assumed performance of our CPU the 8 flops take less than this. The total time is then.

$$\frac{8\text{Flops}}{20\text{ns}} = 400\text{MFlops/sec}$$

## Exercise 3) pow(x, 100)
One approach here is to do a series of multiplications and save some of the intermediate values so we can multiply them together to get x^100. Each time we multiply the value by it self we effectively square the value. To get to 100 we can do this multiple times and save the values for x^4, x^32 and x^64. Then we can multiply these values together.

$$x^{4}x^{32}x^{64} = x^{4+32+64} = x^{100}$$
