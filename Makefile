
# CFLAGS:=$(CFLAGS) -D_DEBUG=1

BASE_SOURCE=memory_flat.h memory_flat.c

test:memory_flat.o test.c 
	$(CC) $(CFLAGS) test.c memory_flat.o -o test; ./test $(LDFLAGS)

memory_flat.o:memory_flat_csym.o
	mv memory_flat_csym.o memory_flat.o

memory_flat_csym.o: $(BASE_SOURCE) memory_flat_csym.c memory_flat_csym.h
	$(CC) $(CFLAGS) memory_flat_csym.c -c

clean:
	rm -f test *.exe *.o