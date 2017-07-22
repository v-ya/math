obj	= main.o debug.o string.o var.o fun.o math.o Calculus.o Bmp.o Wav.o Avi.o

all: math mdl
math: ${obj}
	gcc ${obj} -lm -ldl -o $@
mdl: dl/* sdl/*
	@cd dl/ && make -s && cd ../
	@cd sdl/ && make -s && cd ../
	@echo > mdl
	@echo "make model/*.so"
run: all
	./math Ztest.math
clear:
	rm -f math *.o *.bmp *.wav *.avi model/*.so mdl

install: math
	cp math /usr/bin/math
	chmod 0755 /usr/bin/math

