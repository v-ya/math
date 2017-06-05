obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o Bmp.o Wav.o Avi.o

math: ${obj}
	gcc ${obj} -lm -o $@
run: math
	./math Ztest.math
clear:
	rm -f math *.o *.bmp *.wav *.avi

install: math
	cp math /usr/bin/math
	chmod 0755 /usr/bin/math

