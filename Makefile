obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o

main: ${obj}
	gcc ${obj} -lm -lncursesw -o $@
run: main
	./main test.txt
clear:
	rm -f *.o

