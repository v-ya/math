../model/sdl.so: sdl.c ../dl/dl.c
	gcc $^ -fPIC -shared -o $@ -lSDL
	echo "make $@"
clear:
	rm -f *.o

