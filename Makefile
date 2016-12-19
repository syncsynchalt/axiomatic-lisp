SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -g -std=c99 -Wall -D_BSD_SOURCE

all: axiom

axiom: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: axiom
	./axiom < test.lisp

clean:
	rm -f $(OBJ) axiom
