
examples: lib/libglad.a lib/libGLFW.a
	make -C examples

lib/libGLFW.a: 
	make -C src
	mkdir -p lib
	mv src/libGLFW.a lib

lib/libglad.a:
	make -C deps libglad.a
	mkdir -p lib
	mv deps/libglad.a lib

