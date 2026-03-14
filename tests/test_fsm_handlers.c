#include "lock.h"
#include "unity.h"
#include "fsm_handlers.h"
#include "Mockdoor_sensor.h"
#include "Mockfsm.h"
#include "Mocklock.h"
#include "test_utils.h"
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/timerfd.h>

void setUp(void) {
    Mockdoor_sensor_Init();
    Mockfsm_Init();
}

void tearDown(void) {
    Mockdoor_sensor_Verify();
    Mockfsm_Verify();
    Mockdoor_sensor_Destroy();
    Mockfsm_Destroy();
}

/* --- Action Tests --- */

void test_do_inc_attempts_calls_increment_and_logs(void) {
    fsm_inc_attempts_Expect();
    // Assuming the syslog print fetches the new count
    fsm_get_attempts_ExpectAndReturn(1);

    do_inc_attempts();
}

void test_do_silence_reset_resets_counters(void) {
    fsm_reset_attempts_Expect();
    lock_set_angle_Expect(DOOR_UNLOCKED_ANGLE);
    fsm_get_timer_fd_ExpectAndReturn(10);
    fsm_get_unlock_timeout_CMockExpectAndReturn(__LINE__, (struct timespec){.tv_sec = 30, .tv_nsec = 0});
    fsm_get_timer_fd_ExpectAndReturn(10);
    fsm_get_unlock_timeout_CMockExpectAndReturn(__LINE__, (struct timespec){.tv_sec = 30, .tv_nsec = 0});

    do_silence_reset();
}

void test_do_retry_timer_sets_correct_timeout(void) {
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct timespec timeout = { .tv_sec = 15, .tv_nsec = 0 };

    // 2. Mock the FSM getters to return the REAL fd
    fsm_get_timer_fd_ExpectAndReturn(fd);
    fsm_get_unlock_timeout_ExpectAndReturn(timeout);
    // Note: Since timerfd_settime is a system call, Unity/CMock won't mock it
    // unless you use a wrapper or a linker trick. For now, we verify the
    // fsm_get calls are made to prepare the timer.
    do_retry_timer();
    TEST_ASSERT_EQUAL_STRING("Unlock timer started", last_syslog_msg);
    close(fd);
}

/* --- Door Sensor Guard Tests --- */

void test_guard_is_door_closed_returns_true_when_closed(void) {
    door_sensor_is_open_ExpectAndReturn(false);
    TEST_ASSERT_TRUE(guard_is_door_closed());
}

void test_guard_is_door_open_returns_true_when_open(void) {
    door_sensor_is_open_ExpectAndReturn(true);
    TEST_ASSERT_TRUE(guard_is_door_open());
}

/* --- Max Attempt Guard Tests --- */

void test_guard_is_max_attempts_returns_true_at_limit(void) {
    // Assuming MAX_ATTEMPTS is 3
    fsm_get_attempts_ExpectAndReturn(3);
    TEST_ASSERT_TRUE(guard_is_max_attempts());
}

void test_guard_is_max_attempts_returns_false_below_limit(void) {
    fsm_get_attempts_ExpectAndReturn(2);
    TEST_ASSERT_FALSE(guard_is_max_attempts());
}

void test_guard_is_not_max_attempts_returns_true_below_limit(void) {
    fsm_get_attempts_ExpectAndReturn(2);
    TEST_ASSERT_TRUE(guard_is_not_max_attempts());
}

void test_guard_is_not_max_attempts_returns_false_at_limit(void) {
    fsm_get_attempts_ExpectAndReturn(3);
    TEST_ASSERT_FALSE(guard_is_not_max_attempts());
}

void test_do_lock_logs_locking(void) {
    clear_log_buffer();
    lock_set_angle_Expect(DOOR_LOCKED_ANGLE);
    do_lock();
    TEST_ASSERT_EQUAL_STRING("Locking door", last_syslog_msg);
}

void test_do_unlock_logs_locking(void) {
    clear_log_buffer();
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    lock_set_angle_Expect(DOOR_UNLOCKED_ANGLE);
    fsm_get_timer_fd_ExpectAndReturn(fd);
    fsm_get_unlock_timeout_CMockExpectAndReturn(__LINE__, (struct timespec){.tv_sec = 30, .tv_nsec = 0});
    do_unlock();
    TEST_ASSERT_EQUAL_STRING("Unlock timer started", last_syslog_msg);
    close(fd);
}

void test_do_trigger_alarm_logs_alarm(void) {
    clear_log_buffer();
    do_trigger_alarm();
    TEST_ASSERT_EQUAL_STRING("ALARM TRIGGERED: Unauthorized Access Attempt", last_syslog_msg);
}

void test_do_notify_close_logs_notification(void) {
    clear_log_buffer();
    do_notify_close();
    TEST_ASSERT_EQUAL_STRING("Please close the door", last_syslog_msg);
}

void test_do_retry_timer_failure_path_logs_error(void) {
    clear_log_buffer();
    int fake_fd = -1;  // Invalid FD to force timerfd_settime to fail
    struct timespec timeout = { .tv_sec = 15, .tv_nsec = 0 };

    fsm_get_timer_fd_ExpectAndReturn(fake_fd);
    fsm_get_unlock_timeout_ExpectAndReturn(timeout);

    do_retry_timer();
    TEST_ASSERT_EQUAL_STRING("Failed to start unlock timer", last_syslog_msg);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_do_inc_attempts_calls_increment_and_logs);
    RUN_TEST(test_do_silence_reset_resets_counters);
    RUN_TEST(test_do_retry_timer_sets_correct_timeout);
    RUN_TEST(test_do_lock_logs_locking);
    RUN_TEST(test_do_unlock_logs_locking);
    RUN_TEST(test_do_trigger_alarm_logs_alarm);
    RUN_TEST(test_do_notify_close_logs_notification);
    RUN_TEST(test_do_retry_timer_failure_path_logs_error);
    RUN_TEST(test_guard_is_door_closed_returns_true_when_closed);
    RUN_TEST(test_guard_is_door_open_returns_true_when_open);
    RUN_TEST(test_guard_is_max_attempts_returns_true_at_limit);
    RUN_TEST(test_guard_is_max_attempts_returns_false_below_limit);
    RUN_TEST(test_guard_is_not_max_attempts_returns_true_below_limit);
    RUN_TEST(test_guard_is_not_max_attempts_returns_false_at_limit);
    return UNITY_END();
}
