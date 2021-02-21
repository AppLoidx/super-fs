all: interactive_mode.o main.o
	ld -relocatable main.o interactive_mode.o -o core.o

main.o:
	gcc -c src/main.c -o main.o

interactive_mode.o:
	gcc -c src/interactive_mode.c -o interactive_mode.o

clean:
	rm *.o
