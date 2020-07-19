## Oppgave 2) Prefetching

```C
double s = 0.;
for (i=0; i<N; i++)
    s = s + A[i]*B[i];
```

# (a) What is the expected performance for this loop kernel without data prefetching?

Since a cache line holds consecutive elements in memory one cache line must be fetched for each array. At this point there will be data in the cache to complete four multiplications, and four additions, for a total of 8 flops.

The total number of bytes on one cache line is 32 (4 double precision floats). If we divide by the bandwidth we get

$$\frac{32\text{Bytes}}{3.2\text{GBytes/sec}} = 10\text{ns}$$

The latency to load one cache line is 100ns. The total for both cache lines is then 220ns. Since the flops in this situation is negligible compared to the memory management the performance is approximately \(8\text{Flops}/220\text{ns} = 36\text{MFlops/sec}\)

# (b) Assuming that the CPU is capable of prefetching (loading cache lines from memory in advance so that they are present when needed), what is the required number of outstanding prefetches the CPU has to sus- tain in order to make this code bandwidth-limited, instead of latency- limited?

From **a)** we know that the time used to load one cache line is 10ns. Using eq. 1.6 from the textbook we then get

$$P = 1 + \frac{100\text{ns}}{10\text{ns}} = 11$$

A total of 11 outstanding prefetches are necessary.

# (c) How would this number change if the cache line were twice or four times as long?

For line lengths two and four times as long, the transfer time without latency is 20ns and 40ns respectively. Inserting this into eq. 1.6 we get

$$P_2 = 1 + \frac{100\text{ns}}{20\text{ns}} = 6$$

$$P_4 = 1 + \frac{100\text{ns}}{40\text{ns}} = 3.5$$

Of course we can't do half of a prefetch so \(P_4 = 4\).

# (d) What is the expected performance of the dot-product calculation if we can assume that prefetching hides all the latency?

Without latency we can load two cache lines in 20ns. With the assumed performance of our CPU the 8 flops take less than this. The total time is then.

$$\frac{8\text{Flops}}{20\text{ns}} = 400\text{MFlops/sec}$$
