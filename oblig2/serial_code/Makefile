CC = mpicc
CFLAGS = -O2
LDFLAGS = -L../simple-jpeg/ -lsimplejpeg

PROJ = serial_main
OBJS = serial_main.o
all: simple-jpeg $(PROJ)

serial_main : main.o functions.o
	$(CC) $(CFLAGS) serial_main.o functions.o -o serial_main $(LDFLAGS)

functions.o : functions.c
	$(CC) $(CFLAGS) -c functions.c

main.o : serial_main.c functions.h
	$(CC) $(CFLAGS) -c serial_main.c

simple-jpeg:
	$(MAKE) -C ../simple-jpeg

obj-clean:
	$(RM) *.o

exec-clean:
	$(RM) $(PROJ)

autosave-clean:
	$(RM) *~

clean:
	$(MAKE) obj-clean
	$(MAKE) exec-clean
	$(MAKE) autosave-clean
	$(MAKE) -C ../simple-jpeg clean
