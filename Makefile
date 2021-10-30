src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -L ../libmus
CFLAGS = -DDEBUG -Wall -g -I ../libmus

a.out: $(obj)
	$(CC) -o $@ $^ -lmus $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) a.out
