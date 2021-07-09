#ifndef __RENDER_H__
#define __RENDER_H__

/* All of these return 0 on success, and a non-zero error code on failure */
/* They should log something on an error */
#define RENDER_ERR_UNCONFIGURED 1
#define RENDER_ERR_BADSHADER 1

int render_configure();
int render_initialize();
int render_loop();
int render_terminate();
int render_should_close();

/* The modes */
int render_ctriangle();
int render_look();

#endif
