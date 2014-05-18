CC = gcc
CFLAGS =-Wall
DEPS = util/metadata.h util/usermenu.h
DEPS_O = util/metadata.o util/usermenu.o

OBJMAIN = $(DEPS_O) main.o
OBJSHARER = $(DEPS_O) sharer.o

all: main sharer
	@echo "Done compiling all."

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJMAIN)
	$(CC) -o $@ $^ $(CFLAGS)

sharer: $(OBJSHARER)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f util/*.o
	rm -f main
	rm -f sharer