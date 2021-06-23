DEPS = render.h log.h
#OBJ = parse.o stb_image.o
OBJ = render.o log.o
TESTOBJ = test.o
MAINOBJ = main.o
GLFWLIB = ../lib/libglfw3.a
GLEWLIB = ../lib/libGLEW.a 
READLINELIB = ../lib/libreadline.a 
SOILLIB = ../lib/libSOIL.a 
CFLAGS = -DGLEW_STATIC  -I ../glfw/include -I ../glew/include -I ../readline/include -I ../soil/include
LDFLAGS_OSX = -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lcurses

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ) $(MAINOBJ)
	$(CC) -o ./game $(OBJ) $(MAINOBJ) $(GLFWLIB) $(GLEWLIB) $(READLINELIB) $(SOILLIB) $(LDFLAGS_OSX)

test: $(OBJ) $(TESTOBJ)
	$(CC) -o ./test $(OBJ) $(TESTOBJ)
	./test 

run: game
	./game

clean:	
	rm -f *.o game test bmp.c bmp.h
