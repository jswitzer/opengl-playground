#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <glmc.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
//#include "stb_image.h"
#include "render.h"
#include "log.h"
#include "module.h"

char g_running = 1;

// Event loop 
pthread_t g_console_tid;     // thread id of console reader
int       g_console_pipe[2]; // wake pipe for IPC to event loop

// The function that'll get passed each line of input
void my_rlhandler(char* line);

void my_rlhandler(char* line){
  if(line==NULL){
        // Ctrl-D will allow us to exit nicely
    printf("\nNULLBURGER\n");
    g_running = 0;
  }else{
    if(*line!=0){
          // If line wasn't empty, store it so that uparrow retrieves it
      add_history(line);
    }
    if (!strcmp(line, "testmodule")) { 
      module_test();
    } else {
      printf("Your input was:\n%s\n", line);
    }

    free(line);
  }
}

static void *console_thread(void * arg)  {
    struct timeval tv;
    int retval;
    fd_set rfds;
    const char *prompt = "WOOP> ";
    rl_callback_handler_install(prompt, (rl_vcpfunc_t*) &my_rlhandler);
    while (g_running) {

        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(g_console_pipe[0], &rfds);

        /* Wait up to five seconds. */

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            //log_error("select()");
        } else if (retval) {
            if (FD_ISSET(g_console_pipe[0], &rfds)) {
                char ignore;
                read(g_console_pipe[0], (void*)&ignore, 1);
                // Something else has run the bell, probably we exit now
            }
            if (FD_ISSET(0, &rfds)) {
                rl_callback_read_char();
            }
        } else {
            log_debug("No data within five seconds.\n");
        }
    }
    rl_callback_handler_remove();
    return NULL;
}


int poll_stdin() {
  return 0;
}

int console_setup() {
    pipe(g_console_pipe);
    int err;
    err = pthread_create(&g_console_tid, NULL, &console_thread, NULL);
    if (err != 0) log_error("Couldn't start input handling thread");
    return 0;
}

int console_teardown() {
    //int rc = pthread_kill(g_console_thread_id, SIGUSR1);
    //printf("GOT RC %d from pthread_kill\n", rc);
    // Poke the thread
    char ignore = 'b';
    write(g_console_pipe[1], (void*)&ignore, 1);
    assert(g_running == 0);
    void * res;
    int err = pthread_join(g_console_tid, &res);
    if (err != 0) log_error("Couldn't Join input handling thread");
    return 0;
}

int main() {
    log_info("Starting program\n");

    console_setup();
    render_configure();
    render_initialize();

    render_ctriangle();

    while(!render_should_close() && g_running)
    {
        render_loop();
    }
    render_terminate();
    g_running = 0;
    console_teardown();

}
