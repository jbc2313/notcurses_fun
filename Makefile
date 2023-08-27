# @file
# @version 0.1

# CFLAGS -c was replaced with -g...

CC=gcc -g
CFLAGS= -c -Wall
LDFLAGS=
SOURCES=main.c
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ `pkg-config --libs notcurses`

.cc.o:
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags notcurses`

clean:
	rm -rf *.o main

# end
