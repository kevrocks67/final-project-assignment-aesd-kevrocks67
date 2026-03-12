#include "unity.h"
#include "fsm.h"
#include <stdint.h>
#include "Mockdoor_sensor.h"

// Mock values for initialization
#define MOCK_TIMER_FD 10
#define MOCK_TIMEOUT_SEC 30

void setUp(void) {
    // Reset the singleton state before every test
    fsm_init(MOCK_TIMER_FD, MOCK_TIMEOUT_SEC);
}

void tearDown(void) {
}

void test_fsm_initialization(void) {
    TEST_ASSERT_EQUAL(STATE_LOCKED, fsm_get_state());
    TEST_ASSERT_EQUAL(MOCK_TIMER_FD, fsm_get_timer_fd());
    struct timespec timeout = fsm_get_unlock_timeout();
    TEST_ASSERT_EQUAL(MOCK_TIMEOUT_SEC, timeout.tv_sec);
    TEST_ASSERT_EQUAL(0, timeout.tv_nsec);
}

void test_fsm_invalid_pin_at_max_triggers_alarm(void) {
    // Set up: two failed attempts already
    for (int i = 1; i <= MAX_ATTEMPTS; i++) {
        fsm_inc_attempts();
    }
    TEST_ASSERT_EQUAL(3, fsm_get_attempts());

    // Third invalid PIN should trigger alarm
    fsm_update(EVENT_PIN_INVALID);
    TEST_ASSERT_EQUAL(STATE_ALARM, fsm_get_state());
}

void test_fsm_forced_door_open_triggers_alarm(void) {
    fsm_update(EVENT_DOOR_OPEN);
    TEST_ASSERT_EQUAL(STATE_ALARM, fsm_get_state());
}

void test_fsm_valid_pin_with_door_open_notifies(void) {
    door_sensor_is_open_ExpectAndReturn(true);
    door_sensor_is_open_ExpectAndReturn(true);
    fsm_update(EVENT_PIN_VALID);
    TEST_ASSERT_EQUAL(STATE_LOCKED, fsm_get_state());
}

void test_fsm_unlocked_door_closed_locks(void) {
    // First unlock
    door_sensor_is_open_ExpectAndReturn(false);
    fsm_update(EVENT_PIN_VALID);
    TEST_ASSERT_EQUAL(STATE_UNLOCKED, fsm_get_state());

    // Then close door
    fsm_update(EVENT_DOOR_CLOSED);
    TEST_ASSERT_EQUAL(STATE_LOCKED, fsm_get_state());
}

void test_fsm_alarm_valid_pin_silences_and_resets(void) {
    // Trigger alarm via forced door open
    fsm_update(EVENT_DOOR_OPEN);
    TEST_ASSERT_EQUAL(STATE_ALARM, fsm_get_state());

    // Valid PIN should silence and reset attempts
    fsm_update(EVENT_PIN_VALID);
    TEST_ASSERT_EQUAL(STATE_LOCKED, fsm_get_state());
    TEST_ASSERT_EQUAL(0, fsm_get_attempts());
}

void test_fsm_unlocked_timeout_door_open_restarts_timer(void) {
    // First unlock
    door_sensor_is_open_ExpectAndReturn(false);
    fsm_update(EVENT_PIN_VALID);
    TEST_ASSERT_EQUAL(STATE_UNLOCKED, fsm_get_state());

    // Timeout with door still open should stay unlocked (and call do_retry_timer)
    // Have to mock this function twice since its called twice when going through
    // the transition table
    door_sensor_is_open_ExpectAndReturn(true);
    door_sensor_is_open_ExpectAndReturn(true);
    fsm_update(EVENT_TIMEOUT);
    TEST_ASSERT_EQUAL(STATE_UNLOCKED, fsm_get_state());
}

void test_fsm_ignores_invalid_event(void) {
    // Assuming PIN_CANCEL in LOCKED state doesn't change state in your table
    fsm_update(EVENT_PIN_CANCEL);
    TEST_ASSERT_EQUAL(STATE_LOCKED, fsm_get_state());
}

void test_fsm_state_to_string_conversion(void) {
    TEST_ASSERT_EQUAL_STRING("STATE_LOCKED", fsm_state_to_string(STATE_LOCKED));
    TEST_ASSERT_EQUAL_STRING("STATE_UNLOCKED", fsm_state_to_string(STATE_UNLOCKED));
    TEST_ASSERT_EQUAL_STRING("STATE_ALARM", fsm_state_to_string(STATE_ALARM));
    TEST_ASSERT_EQUAL_STRING("STATE_UNKNOWN", fsm_state_to_string(99));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fsm_initialization);
    RUN_TEST(test_fsm_invalid_pin_at_max_triggers_alarm);
    RUN_TEST(test_fsm_forced_door_open_triggers_alarm);
    RUN_TEST(test_fsm_valid_pin_with_door_open_notifies);
    RUN_TEST(test_fsm_unlocked_door_closed_locks);
    RUN_TEST(test_fsm_alarm_valid_pin_silences_and_resets);
    RUN_TEST(test_fsm_unlocked_timeout_door_open_restarts_timer);
    RUN_TEST(test_fsm_ignores_invalid_event);
    RUN_TEST(test_fsm_state_to_string_conversion);
    return UNITY_END();
}
