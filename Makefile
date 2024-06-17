FLAGS=-Wall -Werror
TOP=./Source
CC=gcc
TESTS=

all: test1 test2

test1: main.c $(TOP)/*.c
	$(CC) $(FLAGS) -DTEST1 -I$(TOP) main.c $(TOP)/*.c -L$(TOP) -lprog2 -o $@
	./$@ > ./$@.out 2> ./$@.err
.PHONY: test1
 
test2: main.c $(TOP)/*.c
	$(CC) $(FLAGS) -DTEST2 -I$(TOP) main.c $(TOP)/*.c -L$(TOP) -lprog2 -o $@
	./$@ > ./$@.out 2> ./$@.err 
.PHONY: test2

clean:
	rm -f *.o test1 test2
