void config_parseargs(int argc, char * argv[]);

/* Test if a config option is true/false */
int config_true(char boolean_option);
int config_false(char boolean_option);

/* These are all get functions */
char * config_error();
char config_fullscreen();
char config_headless();
int  config_xres();
int  config_yres();
char * config_windowtitle();
void config_print_help();

