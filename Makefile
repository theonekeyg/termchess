CC = /usr/bin/gcc
CFLAGS = 
LIBS = -lncurses
DEBUG = -g
BIN = termchess
OBJECTS = $(addprefix obj/, termchess.o options.o)

VPATH = src/

obj/%.o: %.c
	@mkdir -p obj
	$(CC) -c -o $@ $(LIBS) $(CFLAGS) $(DEBUG) $<

all: $(OBJECTS)
	$(CC) -o $(BIN) $^ $(LIBS) $(CFLAGS) $(DEBUG)

clean:
	rm -rf obj termchess
