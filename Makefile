CC=gcc

CFLAGS=-Wextra -Wall -Werror -std=gnu99 #-fsanitize=address -g -O0
LDLIBS=-lSDL -lSDL_image

EXEC=stagenometry

SRC=$(wildcard *.c)

OBJ=$(SRC:.c=.o)

all: $(EXEC)

stagenometry: $(OBJ)
	$(CC)  -o $(EXEC) $(OBJ) $(CFLAGS) $(LDLIBS)


%.o: %.c
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ -c $<

clean:
	rm $(EXEC) -rf


proper:
	rm *.o $(EXEC) -rf
