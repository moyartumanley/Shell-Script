# Makefile for Homework 7
# Jonah Zimmer
CC = gcc
CFLAGS = -Wall -msse3

all: shell forever

shell: shell.c
	${CC} ${CFLAGS} -o shell shell.c -lm

forever: executeForever.c
	${CC} ${CFLAGS} -o forever executeForever.c -lm
clean:
	rm -f shell forever