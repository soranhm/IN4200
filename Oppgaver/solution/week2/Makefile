CC := gcc
FLAGS := -Wall
PROGRAMS := w2_e1_binary_rw.exe w2_e2_memcpy.exe w2_e3_solve.exe

.PHONY : all clean

all: $(PROGRAMS)

w2_e1_binary_rw.exe : w2_e1_binary_rw.c
	$(CC) $(FLAGS) w2_e1_binary_rw.c -o w2_e1_binary_rw.exe

w2_e2_memcpy.exe : w2_e2_memcpy.c
	$(CC) $(FLAGS) w2_e2_memcpy.c -o w2_e2_memcpy.exe

w2_e3_solve.exe : w2_e2_memcpy.c
	$(CC) $(FLAGS) w2_e3_solve.c -o w2_e3_solve.exe -lm -O3

clean :
	rm -f *.o $(PROGRAMS)
