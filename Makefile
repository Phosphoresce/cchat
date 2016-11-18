GCC=gcc -ansi -pedantic -Wall -o chatServer

all: build

build:
	$(GCC) chatServer.c

run: build
	./chatServer 7777 1

clean:
	rm chatServer
