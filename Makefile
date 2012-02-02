
CC = gcc
CFLAGS = -Wall -O2 -g -p
LDFLAGS = -p

OBJS = char_ref.o stack.o test1.o token.o tree.o util.o misc.o format.o parser.o

test: $(OBJS)
	$(CC) $(LDFLAGS) -o test $(OBJS)

clean:
	rm -f $(OBJS)
