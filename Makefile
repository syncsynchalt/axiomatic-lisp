SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -std=c11 -Wall -Werror -D_BSD_SOURCE
ifeq ($(shell uname -s), Darwin)
	CFLAGS += -glldb
else
	CFLAGS += -ggdb
endif

all: axiom

axiom: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: axiom
	@for i in tests/*.lisp; do \
		echo "x Testing $$i"; \
		./axiom < $$i > /tmp/$$$$.txt; \
		if [ $$? != "0" ]; then \
			let fails++; \
		else \
			diff -u $$i.expect /tmp/$$$$.txt 1>&2; \
			if [ $$? != "0" ]; then \
				let fails++; \
			fi; \
		fi \
	done; \
	if [ -n "$$fails" ]; then \
		echo "*** $$fails tests failed ***" 1>&2; \
		exit 1; \
	fi

clean:
	rm -f $(OBJ) axiom
