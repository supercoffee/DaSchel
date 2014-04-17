all: main
main:linkedlist.o cscd340s14hw3.o
	gcc -g cscd340s14hw3.o linkedlist.o -o daschel
linkedlist.o: linkedlist/linkedlist.c
	gcc -c linkedlist/linkedlist.c
cscd340s14hw3.o: cscd340s14hw3.c
	gcc -c cscd340s14hw3.c