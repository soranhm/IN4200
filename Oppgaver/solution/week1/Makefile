ex1a:
	gcc ex1a.c -o ex1a

ex1b:

	gcc ex1b.c -o ex1b

ex1c:
	gcc ex1c.c -o ex1c

ex1c_optimised:
	gcc -O3 ex1c.c -o ex1c_optimised

ex1d:
	gcc ex1d.c -o ex1d

ex2a:
	gcc ex2a.c ex2a_functions.c -o ex2a -lm

ex3:
	gcc ex3.c -o ex3

all:
	make ex1a ex1b ex1c ex1c_optimised ex1d ex2a ex3

remove:
	ls | grep -v "\." | grep -v Makefile |xargs rm