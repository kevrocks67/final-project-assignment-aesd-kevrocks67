#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <syslog.h>

// These let your tests inspect what was logged
extern char last_syslog_msg[1024];
extern int last_syslog_priority;

// Call this in setUp() to start each test with a clean slate
void clear_log_buffer(void);

#endif
