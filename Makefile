SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -g -std=c11 -Wall -D_BSD_SOURCE

all: axiom

axiom: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: axiom
	@for i in tests/*.lisp; do \
		echo "Testing $$i"; \
		./axiom < $$i > /tmp/$$$$.txt; \
		if [ $$? != "0" ]; then \
			let fails++; \
		fi; \
		diff -u $$i.expect /tmp/$$$$.txt 1>&2; \
		if [ $$? != "0" ]; then \
			let fails++; \
		fi \
	done; \
	if [ -n "$$fails" ]; then \
		echo "*** $$fails tests failed ***" 1>&2; \
		exit 1; \
	fi

clean:
	rm -f $(OBJ) axiom
