SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -g -Wall -Wno-unused-function

all: axiom

#.c.o:
#	$(CC) $(CFLAGS) - $^ -o $@

axiom: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: axiom
	./axiom < test.lisp

clean:
	rm -f $(OBJ) axiom
