CC := gcc-9
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = matvec.h

.PHONY : all clean

all: $(PROGRAMS)

w7_e2_scheduling.exe : w7_e2_scheduling.c
	$(CC) $< -o $@ -fopenmp $(FLAGS)

w7_e3a_matvec.exe : w7_e3a_matvec.c $(DEP)
	$(CC) $< -o $@ $(FLAGS)

w7_e3b_matvec.exe : w7_e3b_matvec.c $(DEP)
	$(CC) $< -o $@ -fopenmp $(FLAGS)

w7_e3c_matvec.exe : w7_e3c_matvec.c $(DEP)
	$(CC) $< -o $@ -fopenmp $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
