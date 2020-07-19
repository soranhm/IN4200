CC := gcc
FLAGS = -Wall

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = oddeven.h

.PHONY : all clean

all: $(PROGRAMS)

w8_e2_output.exe : w8_e2_output.c
	$(CC) $< -o $@ -fopenmp $(FLAGS)

w8_e3_oddeven.exe : w8_e3_oddeven.c $(DEP)
	$(CC) $< -o $@ -fopenmp $(FLAGS)

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
