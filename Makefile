CC=gcc
CFLAGS=-Wall -g

clean:
	rm -rf *.o
	rm -rf virtualmem

virtualmem: virtualmem.o
	$(CC) $(CFLAGS) -o virtualmem virtualmem.o

virtualmem.o: virtualmem.c
	$(CC) $(CFLAGS) -c virtualmem.c
