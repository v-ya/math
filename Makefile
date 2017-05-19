obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o

math: ${obj}
	gcc ${obj} -lm -lncursesw -o $@
run: math
	./math test.txt
clear:
	rm -f *.o

