#include <string.h>
#include <stdio.h>
#include "log.h"

#define MAX_ERROR_LEN 255
#define MAX_PROGNAME_LEN 32
#define MAX_WINDOWTITLE_LEN 1023

/* default options */
char g_config_error[MAX_ERROR_LEN + 1] = {0};
char g_config_headless = 0;
char g_config_fullscreen = 0;
int g_config_xres = 0;
int g_config_yres = 0;
char g_config_configured = 0;
char g_config_windowtitle[MAX_WINDOWTITLE_LEN + 1] = {0};
char g_config_progname[MAX_PROGNAME_LEN + 1] = {0};

void config_set_windowtitle(char * windowtitle) {
    strncpy(g_config_windowtitle, windowtitle, MAX_WINDOWTITLE_LEN);
}

void config_set_error(char * error) {
    strncpy(g_config_error, error, MAX_ERROR_LEN);
}
char * config_error() {
    if (g_config_error[0] == '\0') {
        return NULL;
    } 
    return g_config_error;
}

char config_fullscreen() {
    return g_config_fullscreen;
}

int config_true(char boolean_option) {
    if (boolean_option == 1) {
        return 1;
    } else {
        return 0;
    }
}

int config_false(char boolean_option) {
    if (boolean_option == 0) {
        return 1;
    } else {
        return 0;
    }
}


char config_headless() {
    return g_config_headless;
}

char * config_windowtitle() {
    return g_config_windowtitle;
}

int config_xres() {
    return g_config_xres;
}

int config_yres() {
    return g_config_yres;
}

/* 
 * Set defaults, then 
 * parse the arguments from the command line 
 */
void config_parseargs(int argc, char * argv[]) {
    config_set_windowtitle("Jim's OpenGL Playground");
    g_config_xres = 800;
    g_config_yres = 600;
    g_config_fullscreen = 0;
    g_config_headless = 0;
    g_config_configured = 1;
    for (int i=0; i < argc; i++) {
        char * token = argv[i];
        if (token[0] == '/' || token[0] == '-') {
            switch (token[1]) {
                case '?':
                case 'h':
                    config_set_error("Help requested");
                    break;
                case 'H':
                    g_config_headless = 1;
                    break;
            }
        }
    }
}

void config_print_help() {
    printf("Usage: ./%s <options>\n", g_config_progname);
    printf(" Options:\n");
    printf("    -H : set headless to true\n");
    printf("    -h : help\n");
    printf("    -? : help\n");
}
