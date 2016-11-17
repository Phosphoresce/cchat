GCC=gcc -ansi -pedantic -Wall -o chatServer
CFLAGS=-a -ldflags '-s'

all: build

build:
	$(GCC) chatServer.c

run: build
	./chatServer

stat:
	$(GCC) $(CFLAGS) chatServer.c

clean:
	rm chatServer
