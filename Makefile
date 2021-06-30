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
	make -C soil libSOIL.a
	cp -f soil/build/lib/libSOIL.a lib/
	# Should move this into soil makefile:
	#mkdir -p soil/lib soil/obj

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
	./dep.sh unpack tcc
lua:
	./dep.sh unpack lua
glfw:
	./dep.sh unpack glfw
glew:
	./dep.sh unpack glew
readline:
	./dep.sh unpack readline
	cd readline && ./configure
soil:
	./dep.sh unpack soil
	cd soil && ./configure

