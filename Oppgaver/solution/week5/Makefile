CC := gcc
PROFILE := -pg

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))
DEP = week5.h

.PHONY : all clean

all: $(PROGRAMS)

w5_e2_unroll.exe : w5_e2_unroll.c $(DEP)
	$(CC) $< -o $@

w5_e3_optimize.exe : w5_e3_optimize.c $(DEP)
	$(CC) $< -o $@ -lm

clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
