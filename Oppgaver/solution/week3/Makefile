CC := gcc
PROFILE := -pg

PROGRAMS = $(patsubst %.c,%.exe,$(wildcard *.c))

.PHONY : all clean

all: $(PROGRAMS)

w3_e1_division.exe : w3_e1_division.c
	$(CC) $< -o $@ -O0 $(PROFILE)

w3_e3_pow.exe : w3_e3_pow.c
	$(CC)  $< -o $@ -lm

# w3_e3_pow.exe : w3_e3_pow.c#
# 	$(CC) -I/usr/local/include $< /usr/local/lib/libpapi.a -o $@ -lm


clean :
	rm -f *.o $(PROGRAMS)

print-%  : ; @echo $* = $($*)
