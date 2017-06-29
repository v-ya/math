obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o Bmp.o Wav.o Avi.o

math: ${obj}
	gcc ${obj} -lm -ldl -o $@
mdl:
	cd dl/ && make && cd ../
run: math
	./math test.math
clear:
	rm -f math *.o *.bmp *.wav *.avi model/*.so

install: math
	cp math /usr/bin/math
	chmod 0755 /usr/bin/math

