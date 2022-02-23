
#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LOG_MESSAGE_SZ 256

static inline __attribute__((always_inline)) void log_message(const char *msg, const char *fname, int line)
{
        struct tm *time_s;
        int len = 0;
        char buff[LOG_MESSAGE_SZ] = {0};

        /* Get current time */
        time_t now = time(NULL);

        time_s = gmtime(&now);

        len = strftime(buff, LOG_MESSAGE_SZ, "[%Y-%m-%dT%H:%M:%S] ", time_s);

        /* format the rest of the message */
        sprintf(buff + len, "[%d] %s", getpid(), msg);

        /* print to file */
        fprintf(stdout, "%s - %s line %i\n", buff, fname, line);
        return;
}

static inline void log_message_args(const char *msg, const char *fname, int line, ...)
{
  /* format the message */
  va_list list;

  char buff[LOG_MESSAGE_SZ] = {};
  va_start(list, line);
  vsprintf(buff, msg, list);
  va_end(list);
  log_message(buff, fname, line);

  return;
}

#define LOG_MESSAGE(msg) log_message(msg, __FILE__, __LINE__)
#define LOG_MESSAGE_ARGS(msg, ...)                                    \
  log_message_args(msg, __FILE__, __LINE__, __VA_ARGS__)


#endif	/* __LOG_H__ */
