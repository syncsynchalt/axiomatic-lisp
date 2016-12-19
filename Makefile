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
			echo "*** failure running $$i ***" 1>&2; \
			exit 1; \
		fi; \
		diff -u $$i.expect /tmp/$$$$.txt 1>&2; \
		if [ $$? != "0" ]; then \
			echo "*** failure in output of $$i ***" 1>&2; \
			exit 1; \
		fi \
	done

clean:
	rm -f $(OBJ) axiom
