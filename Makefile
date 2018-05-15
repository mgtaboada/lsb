CC=gcc
CFLAGS=-Wall


all:lsb

lsb: lsb.c
	$(CC) $(CFLAGS) -o $@ lsb.c
