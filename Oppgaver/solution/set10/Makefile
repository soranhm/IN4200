CC := gcc
MPICC := mpicc
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = set10.h

.PHONY : all clean

all: $(PROGRAMS)

w10_e1_hello.exe : w10_e1_hello.c
	$(MPICC) $< -o $@ $(FLAGS)

w10_e2_reduce.exe : w10_e2_reduce.c
	$(MPICC) $< -o $@ $(FLAGS)

w10_e3_matvec.exe : w10_e3_matvec.c $(DEP)
	$(MPICC) $< -o $@ $(FLAGS)

w10_e4_coldecomp.exe : w10_e4_coldecomp.c $(DEP)
	$(MPICC) $< -o $@ $(FLAGS)

README.pdf : README.md
	pandoc -o $@ $<

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
