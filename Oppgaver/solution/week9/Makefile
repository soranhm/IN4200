CC := gcc
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = week9.h

.PHONY : all clean

all: $(PROGRAMS)

w9_e1_generalization.exe : w9_e1_generalization.c
	$(CC) $< -o $@ $(FLAGS)

w9_e2_errors.exe : w9_e2_errors.c
	$(CC) $< -o $@ -fopenmp $(FLAGS)

w9_e3_parallel.exe : w9_e3_parallel.c
	$(CC) $< -o $@ -fopenmp $(FLAGS) -lm

w9_e4_graph.exe : w9_e4_graph.c $(DEP)
	$(CC) $< -o $@ $(FLAGS) -fopenmp

README.pdf : README.md
	pandoc -o $@ $<

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
