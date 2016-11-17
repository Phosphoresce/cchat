GCC=gcc -ansi -pedantic -Wall -o cchat
CFLAGS=-a -ldflags '-s'

all: build

build:
	$(GCC) cchat.c

run: build
	./cchat

stat:
	$(GCC) $(CFLAGS) cchat.c

clean:
	rm cchat
