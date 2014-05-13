CC = gcc
CFLAGS =-Wall
DEPS = util/metadata.h
OBJ = util/metadata.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f util/*.o
	rm -f main