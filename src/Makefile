DEPS = stb_image.h
OBJ = parse.o stb_image.o
TESTOBJ = test.o
MAINOBJ = main.o
GLFWLIB = ../lib/libglfw3.a
GLEWLIB = ../lib/libGLEW.a 
CFLAGS = -DGLEW_STATIC  -I ../glfw/include -I ../glew/include 
LDFLAGS_OSX = -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ) $(MAINOBJ)
	$(CC) -o ./game $(OBJ) $(MAINOBJ) $(GLFWLIB) $(GLEWLIB) $(LDFLAGS_OSX)

test: $(OBJ) $(TESTOBJ)
	$(CC) -o ./test $(OBJ) $(TESTOBJ)
	./test 

run: game
	./game

clean:	
	rm -f *.o game test bmp.c bmp.h
