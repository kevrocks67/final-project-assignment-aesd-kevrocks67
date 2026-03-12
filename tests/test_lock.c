#include "unity.h"
#include "lock.h"
#include "test_utils.h"
#include "unity_internals.h"
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

// Mocking state variables
int mock_open_return = 10; // Default "valid" file descriptor
bool mock_write_fails = false;
int usleep_called_with = 0;

// Declare the real functions (provided by the linker via --wrap)
extern int __real_open(const char *pathname, int flags, mode_t mode);
extern ssize_t __real_write(int fd, const void *buf, size_t count);
extern int __real_close(int fd);

// --- Linker Wrappers ---

int __wrap_open(const char *pathname, int flags, ...) {
    // Passthrough for gcov .gcda files - they need real file I/O
    if (pathname && strstr(pathname, ".gcda") != NULL) {
        return __real_open(pathname, flags, 0644);
    }
    return mock_open_return;
}

ssize_t __wrap_write(int fd, const void *buf, size_t count) {
    // Passthrough for gcov file writes
    if (fd >= 100) {  // Real FDs from __real_open are typically >= 3, gcov uses higher FDs
        return __real_write(fd, buf, count);
    }
    if (mock_write_fails) return -1;
    return (ssize_t)count;
}

int __wrap_close(int fd) {
    // Passthrough for gcov file closes
    if (fd >= 100) {
        return __real_close(fd);
    }
    return 0;
}

int __wrap_usleep(unsigned int usec) {
    usleep_called_with = usec;
    return 0;
}

// --- Setup/Teardown ---

void setUp(void) {
    mock_open_return = 10;
    mock_write_fails = false;
    usleep_called_with = 0;
    clear_log_buffer();
}

void tearDown(void) {}

// --- Tests ---

void test_angle_to_duty_cycle_math(void) {
    // Testing your calibrated values from the screenshot
    // 0 deg -> 570,000
    TEST_ASSERT_EQUAL_INT(570000, angle_to_duty_cycle_ns(0));
    // 90 deg -> 1,530,000
    TEST_ASSERT_EQUAL_INT(1530000, angle_to_duty_cycle_ns(90));
    // 180 deg -> 2,490,000
    TEST_ASSERT_EQUAL_INT(2490000, angle_to_duty_cycle_ns(180));

    // Test Clamping
    TEST_ASSERT_EQUAL_INT(570000, angle_to_duty_cycle_ns(-10));
    TEST_ASSERT_EQUAL_INT(2490000, angle_to_duty_cycle_ns(200));
}

void test_lock_init_success(void) {
    bool result = lock_init();

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(250000, usleep_called_with);
    TEST_ASSERT_NOT_NULL(strstr(last_syslog_msg, "initialized successfully"));
}

void test_lock_init_fails_on_period_write(void) {
    // Force the first write (export) to succeed, but second to fail
    // We'll simulate this by saying open fails on the second attempt
    mock_open_return = -1;

    bool result = lock_init();

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_NOT_NULL(strstr(last_syslog_msg, "Failed to set period"));
}

void test_lock_set_and_get_angle(void) {
    lock_set_angle(45);
    TEST_ASSERT_EQUAL_INT(45, lock_get_angle());

    lock_set_angle(180);
    TEST_ASSERT_EQUAL_INT(180, lock_get_angle());
}

void test_lock_cleanup(void) {
    // Cleanup doesn't return anything, just ensure it runs without crashing
    lock_cleanup();
    // In a real test, you'd verify write was called with "0"
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_angle_to_duty_cycle_math);
    RUN_TEST(test_lock_init_success);
    RUN_TEST(test_lock_init_fails_on_period_write);
    RUN_TEST(test_lock_set_and_get_angle);
    RUN_TEST(test_lock_cleanup);
    return UNITY_END();
}
