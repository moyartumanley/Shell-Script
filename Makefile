# Makefile for Homework 7
# Jonah Zimmer
CC = gcc
CFLAGS = -Wall -msse3

all: shell

shell: shell.c
	${CC} ${CFLAGS} -o shell shell.c -lm

clean:
	rm -f shell