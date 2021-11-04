src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -DDEBUG -Wall -g 

a.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) a.out

check: a.out
	cd t && ./runtests

