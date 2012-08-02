# 
# Makefile for distributed shell program
#

CC = gcc
CFLAGS = -Wall
LIBFLAGS =

all: dsh server

server: server.c 
	$(CC) $(CFLAGS) server.c -o server $(LIBFLAGS)

dsh: dsh.c 
	$(CC) $(CFLAGS) dsh.c -o dsh $(LIBFLAGS)

clean:
	/bin/rm -rf *.o core dsh server