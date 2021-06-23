#ifndef __LOG_H__
#define __LOG_H__

#define LOG_ERROR 50
#define LOG_INFO  30
#define LOG_DEBUG 10

void log_log(int priority, char * format, ...);
void log_info(char * format, ...);
void log_debug(char * format, ...);
void log_error(char * format, ...);
int log_getlevel();
void log_setlevel(int level);

#endif
