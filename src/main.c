#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <glmc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
//#include "stb_image.h"
#include "config.h"
#include "render.h"
#include "log.h"
#include "module.h"

#ifdef WIN32
#include <editline/readline.h>
#else
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#endif


char g_running = 1;

#ifdef WIN32
   DWORD g_console_tid;
   HANDLE g_console_threadhandle;
#else
// Event loop 
pthread_t g_console_tid;     // thread id of console reader
int       g_console_pipe[2]; // wake pipe for IPC to event loop
#endif

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
const char * promptstr="WOOP>";
#ifdef WIN32
DWORD WINAPI console_thread(LPVOID Param){
	char *line;
	while ((line = readline(promptstr))
    && g_running) {
		my_rlhandler(line);
  }
}
#else
static void *console_thread(void * arg)  {
    struct timeval tv;
    int retval;
    fd_set rfds;
    rl_callback_handler_install(promptstr, (rl_vcpfunc_t*) &my_rlhandler);
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
#endif


int poll_stdin() {
  return 0;
}

int console_setup() {
#ifdef WIN32
   int Param = 0;
   /* create the thread */
   g_console_threadhandle = CreateThread( NULL, /* default security attributes */ 0, /* default stack size */    
   console_thread, /* thread function */ &Param, /* parameter to thread function */ 0, /* default creation    flags */ &g_console_tid);
   /* returns the thread identifier */
   return 0;
#else
    pipe(g_console_pipe);
    int err;
    err = pthread_create(&g_console_tid, NULL, &console_thread, NULL);
    if (err != 0) log_error("Couldn't start input handling thread");
    return 0;
#endif
}

int console_teardown() {
    //int rc = pthread_kill(g_console_thread_id, SIGUSR1);
    //printf("GOT RC %d from pthread_kill\n", rc);
    // Poke the thread
#ifdef WIN32
    if (g_console_threadhandle != NULL){

		    DWORD dwTmp;
            // Send a keypress to wake up the readline() function
    INPUT_RECORD ir[2];
    ir[0].EventType = KEY_EVENT;
    ir[0].Event.KeyEvent.bKeyDown = TRUE;
    ir[0].Event.KeyEvent.dwControlKeyState = 0;
    ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
    ir[0].Event.KeyEvent.wRepeatCount = 1;
    ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
    ir[1] = ir[0];
    ir[1].Event.KeyEvent.bKeyDown = FALSE;
    WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dwTmp);

      /* now wait for the thread to finish */ WaitForSingleObject(g_console_threadhandle,INFINITE);
      /* close the thread handle */
      CloseHandle(g_console_threadhandle);
   }
#else
    char ignore = 'b';
    write(g_console_pipe[1], (void*)&ignore, 1);
    assert(g_running == 0);
    void * res;
    int err = pthread_join(g_console_tid, &res);
    if (err != 0) log_error("Couldn't Join input handling thread");
#endif
    return 0;
}

int main(int argc, char * argv[]) {
    char * arghelp = 0;
    log_info("Parsing args\n");
    config_parseargs(argc, argv);
    if (config_error() != NULL) {
        printf("Config error: \n  %s \n\n", config_error());
        config_print_help();
        return 1;
    }
    log_info("Starting program\n");
    console_setup();
    if (config_false(config_headless())) {
        render_configure();
        render_initialize();

        render_ctriangle();

        while(!render_should_close() && g_running)
        {
            render_loop();
        }
        render_terminate();
        g_running = 0;
    } else {
        while(g_running)
        {
            sleep(1);
        }
    }
    console_teardown();
#ifdef WIN32
    printf("Press any key to continue");
    getchar();
#endif
}
