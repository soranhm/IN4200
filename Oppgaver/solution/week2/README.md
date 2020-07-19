# README week 2 of IN3200

To compile all the programs for this week:
```
$ make all
```

## Makefile
The makefile for this week show how to use variables and how to define `.PHONY` targets.

### Variables
Variables are defined with `=` and `:=`. Ex.:
```
CC := gcc
FLAGS := -Wall
```
The difference in how these variables are treated is a bit subtle and is about how variables are expanded when the makefile is parsed. For the simple makefiles we are using now you should not notice any difference.


### Phony targets
When a target is defined as `.PHONY`, whatever make program you are using will not expect an output with the target name to be created. This is nice for those operations that don't create programs or other files like `clean` or `all`. These targets can have prerequisites that gets created though. To read more check out the [gnu make manual](https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html).

Ex.:
```
.PHONY : all clean

all: $(PROGRAMS)

clean :
	rm -f *.o $(PROGRAMS)
```


The makefiles will get more and more advanced, so it's a good idea to check them out as the course goes on, unless you already have a good grasp of them.

## Exercise 1)
Coming up with a "fair" test of the read/write speed of ASCII vs. binary is not as easy as it sounds. There are several different ways to read and write to files, both ASCII and binary in C.

Some possible functions you can use are:
```C
// Write:
// ASCII:
int fprintf (FILE *stream, const char *format, ... );
int fputs (const char *str, FILE *stream );
// Binary:
size_t fwrite (const void *ptr, size_t size, size_t count, FILE * stream);

// Read:
// ASCII:
int fscanf(FILE *stream, const char *format, ...);
char *fgets(char *str, int n, FILE *stream);
// Binary:
size_t fread (void *ptr, size_t size, size_t count, FILE *stream);
```

In the `fopen()` function you can specify if you want to open the file for writing ASCII, `"w", "r+"` or binary `"wb", "rb+"`. The different read/write methods does not care about the mode the file was opened in. The flag is only information for the operating system so some minor optimizations can be done depending on the mode. Eg. opening in binary mode and using `fprintf()` to write to the file will still write ASCII.

## Exercise 2)
The `memcpy()` function is defined in the `<string.h>` library and is called:
```C
void *memcpy (void * destination, const void * source, size_t num);
```
The `num` argument is the number of bytes to be copied.

## Exercise 3)
When solving a system like this, you usually need to define a boundary condition (what to do at the edges). It's not clearly stated, but in this exercise we are assuming a constant value at the edges. Notice that the update rule is defined for indexes \(1 \le i \le n_x - 2\), and similar for \(j\) and \(k\). We never actually update the edges, and they stay constant. Dependant on the problem at hand the boundary conditions will change.

See the code for an implementation.

## Exercise 4)
We can simply count the number of operations needed to do one step in our algorithm.

\(u_{i,j,k} = v_{i,j,k} + \frac{v_{i-1,j,k} + v_{i,j-1,k} + v_{i,j,k-1} - 6v_{i,j,k} + v_{i+1,j,k} + v_{i,j+1,k} + v_{i,j,k+1}}{6}\)

There are 9 floating point operations here. 7 add/subtract and 2 multiply (if you code the division as a multiplication, any reason for doing this?).

We need to do this over all the inner indexes, which means \(9(n_x-2)(n_y-2)(n_z-2)\) FLOP for one iteration. Finally we can multiply with the number of iterations and divide by the total time to get the FLOPS.

\(\text{FLOPS} = \frac{9N(n_x-2)(n_y-2)(n_z-2)}{T}\)
