INSTALLED_READLINE_HEADERS = readline.h chardefs.h keymaps.h history.h tilde.h \
                    rlstdc.h rlconf.h rltypedefs.h
src/game: lib
	make -C src

lib: lib/libGLEW.a lib/libglfw3.a lib/libreadline.a lib/libSOIL.a

lib/libglfw3.a: glfw
	mkdir -p lib
	make -C glfw
	cp -f glfw/lib/libGLFW.a lib/libglfw3.a

lib/libGLEW.a: glew
	mkdir -p lib
	make -C glew
	cp -f glew/lib/libGLEW.a lib/

lib/libSOIL.a: soil
	mkdir -p lib
	# Should move this into soil makefile:
	mkdir -p soil/lib soil/obj
	make -C soil
	cp -f soil/lib/libSOIL.a lib/

lib/libreadline.a: readline
	mkdir -p lib
	make -C readline
	cp -f readline/libreadline.a lib/
	mkdir -p readline/include/readline
	for f in ${INSTALLED_READLINE_HEADERS}; do \
		cp readline/$$f readline/include/readline ; \
	done

clean:
	rm -rf tcc lua glfw glew readline lib soil
	make -C src clean

tcc:
	tar -xjf depsrc/tcc-0.9.27.tar.bz2
lua:
	tar -xjf depsrc/lua-5.3.4.tar.bz2
glfw:
	tar -xjf depsrc/glfw-3.2.1.tar.bz2
	./depsrc/glfw-3.2.1.patch/patch.sh ./glfw
glew:
	tar -xjf depsrc/glew-2.1.0.tar.bz2
readline:
	tar -xjf depsrc/readline-8.0.tar.bz2
	cd readline && ./configure
soil:
	tar -xjf depsrc/soil-0.0.2.tar.bz2


