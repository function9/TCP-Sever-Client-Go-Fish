CC = gcc
CFLAGS=-O2 -Wall -g
LDLIBS = -lpthread

all: client server

client: client.c csapp.c csapp.h
server: server.c csapp.c csapp.h

clean:
	rm -rf *~ client server *.o
