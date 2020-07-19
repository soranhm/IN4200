## Exercise 4)
We can simply count the number of operations needed to do one step in our algorithm.

\(u_{i,j,k} = v_{i,j,k} + \frac{v_{i-1,j,k} + v_{i,j-1,k} + v_{i,j,k-1} - 6v_{i,j,k} + v_{i+1,j,k} + v_{i,j+1,k} + v_{i,j,k+1}}{6}\)

There are 9 floating point operations here. 7 add/subtract and 2 multiply (if you code the division as a multiplication, any reason for doing this?).

We need to do this over all the inner indexes, which means \(9(n_x-2)(n_y-2)(n_z-2)\) FLOP for one iteration. Finally we can multiply with the number of iterations and divide by the total time to get the FLOPS.

\(\text{FLOPS} = \frac{9N(n_x-2)(n_y-2)(n_z-2)}{T}\)
