#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

// Global buffer to verify logs in any test
char last_syslog_msg[1024];
int last_syslog_priority;

void syslog(int priority, const char *format, ...) {
    last_syslog_priority = priority;
    va_list args;
    va_start(args, format);
    vsnprintf(last_syslog_msg, sizeof(last_syslog_msg), format, args);
    va_end(args);
}

// Helper to clear the log for the next test
void clear_log_buffer() {
    memset(last_syslog_msg, 0, sizeof(last_syslog_msg));
    last_syslog_priority = 0;
}
