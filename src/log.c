#include <stdarg.h>
#include <stdio.h>
#include "log.h"

int g_log_level = LOG_INFO;

void vlog(int priority, char * format, va_list ap) {
  if (priority >= g_log_level) {
    vprintf(format, ap);
  }
}

void log_log(int priority, char * format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(priority, format, ap);
  va_end (ap);
}

void log_debug(char * format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_DEBUG, format, ap);
  va_end(ap);
}

void log_info(char * format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_INFO, format, ap);
  va_end (ap);
}

void log_error(char * format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_ERROR, format, ap);
  va_end (ap);
}


int log_getlevel() {
  return g_log_level;
}

void log_setlevel(int level) {
  g_log_level = level;
}
