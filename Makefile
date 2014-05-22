CC = gcc
CFLAGS =-Wall -lm -lssl -lcrypto
DEPS = util/metadata.h util/usermenu.h util/constants.h server.h publish.h util/local_file.h util/torrent.h util/hash.h
DEPS_O = util/metadata.o util/usermenu.o publish.o util/local_file.o search.o util/hash.o

OBJMAIN = $(DEPS_O) main.o
OBJSHARER = $(DEPS_O) sharer.o
OBJSERVER = $(DEPS_O) server.o

all: main sharer server
	@echo "Done compiling all."

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJMAIN)
	$(CC) -o $@ $^ $(CFLAGS)

sharer: $(OBJSHARER)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(OBJSERVER)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f util/*.o
	rm -f main
	rm -f sharer
	rm -f server
