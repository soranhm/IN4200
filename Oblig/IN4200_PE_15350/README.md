#       Computing PageRank
# IN4200 Partial Exam, Spring 2019

## Compilation of the program:
gcc-8 -O* -fopenmp PE_main_15350.c
(where * is 0,1 or 2)

I use compiler gcc-8, but it should run in any version as long it have openMP.

The compiler's goal without any optimization option, is to reduce the
cost of compilation and to make debugging produce the expected results.

When turning on optimization flags, the compiler attempt to improve the
performance and/or code size at the expense of compilation time and possibly
the ability to debug the program.

Optimizing compilation takes somewhat more time, and a lot more memory for a
large function.

-- '-O0', Do not optimize. This is the default.

-- '-O', the compiler tries to reduce code size and execution time,
without performing any optimizations that take a great deal of compilation time.

-- '-O2', Optimizes even more. This option increases both compilation time and
the performance of the generated code.

It is few more options but, it doesn't make the program run any faster.
The fastest one is '-O2' which runs the entire program in 0.7 seconds,
while the default '-O0' runs the entire program in 1.2 seconds.

## Choosing number of Threads:
The command to change the number of threads is:

:15350$ export OMP_NUM_THREADS=<number of threads to use>

## Running the program
(the time depends of the pc)
Her is the fastest way running the program with same eps and compilation:

Wrong way:
:15350$ gcc-8 -O2 -fopenmp PE_main_15350.c
:15350$ ./a.out
Filename required.
Running example: ./a.out filename d eps n

Correct way:
:15350$ gcc-8 -O2 -fopenmp PE_main_15350.c
:15350$ ./a.out web-NotreDame.txt 0.85 1e-10 10
