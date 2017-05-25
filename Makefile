obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o Bmp.o Wav.o

math: ${obj}
	gcc ${obj} -lm -o $@
run: math
	./math test.math
clear:
	rm -f *.o math

install: math
	cp math /usr/bin/math
	chmod 0755 /usr/bin/math

