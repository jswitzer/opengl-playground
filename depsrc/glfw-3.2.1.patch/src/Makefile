CINCLUDE=-I ../include
CFLAGS=-D _GLFW_COCOA -D _GLFW_USE_CHDIR -D _GLFW_USE_MENUBAR -D _GLFW_USE_RETINA 

OBJS=cocoa_time.o context.o init.o input.o monitor.o posix_tls.o vulkan.o window.o xkb_unicode.o cocoa_init.o cocoa_joystick.o cocoa_monitor.o cocoa_window.o nsgl_context.o

libGLFW.a : $(OBJS)
	ar rcs $@ $^

cocoa_%.o : cocoa_%.m
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

