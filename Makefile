src/game: lib
	make -C src

lib: lib/libGLEW.a lib/libglfw3.a

lib/libglfw3.a: glfw
	mkdir -p lib
	make -C glfw
	cp -f glfw/lib/libGLFW.a lib/libglfw3.a

lib/libGLEW.a: glew
	mkdir -p lib
	make -C glew
	cp -f glew/lib/libGLEW.a lib/

clean:
	rm -rf tcc lua glfw glew lib

tcc:
	tar -xjf depsrc/tcc-0.9.27.tar.bz2
lua:
	tar -xjf depsrc/lua-5.3.4.tar.bz2
glfw:
	tar -xjf depsrc/glfw-3.2.1.tar.bz2
	./depsrc/glfw-3.2.1.patch/patch.sh ./glfw
glew:
	tar -xjf depsrc/glew-2.1.0.tar.bz2
