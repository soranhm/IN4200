CC := gcc-9
PROFILE := -pg

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c)) w4_e1_fast.exe
DEP = week4.h

.PHONY : all clean

all: $(PROGRAMS)

w4_e1_bandwidth.exe : w4_e1_bandwidth.c $(DEP)
	$(CC) $< -o $@ -fopt-info

w4_e1_fast.exe : w4_e1_bandwidth.c $(DEP)
	$(CC) $< -o $@ -O3 -fopt-info

w4_e2_stride.exe : w4_e2_stride.c $(DEP)
	$(CC) $< -o $@ -O3

w4_e3_matmult.exe : w4_e3_matmult.c $(DEP)
	$(CC) $< -o $@


clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
