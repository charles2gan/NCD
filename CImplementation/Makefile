all: main.o
	gcc -g -o NCD main.o -pthread

main.o: main.c
	gcc -g -c -pthread main.c
clean:
	rm *.o NCD
