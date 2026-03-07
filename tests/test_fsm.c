#include "unity.h"
#include "fsm.h"
#include "fsm_types.h"
#include "fsm_table.h"
#include <stdbool.h>

/* We need access to the internal state to verify it.
 * If current_state is static in fsm.c, you may need an 'fsm_get_state()' getter. */
extern State current_state;

void setUp(void) {
    fsm_init(); // Reset to STATE_LOCKED before every test
}

void tearDown(void) {
}

void test_fsm_valid_pin_unlocks(void) {
    // Arrange: Ensure we start Locked
    TEST_ASSERT_EQUAL(STATE_LOCKED, current_state);

    // Act: Simulate a valid PIN event
    fsm_update(EVENT_PIN_VALID);

    // Assert: State should now be Unlocked
    TEST_ASSERT_EQUAL(STATE_UNLOCKED, current_state);
}

void test_fsm_breach_triggers_alarm(void) {
    fsm_update(EVENT_DOOR_OPEN);
    TEST_ASSERT_EQUAL(STATE_ALARM, current_state);
}

void test_fsm_timeout_relocks(void) {
    // Move to Unlocked first
    fsm_update(EVENT_PIN_VALID);
    TEST_ASSERT_EQUAL(STATE_UNLOCKED, current_state);

    // Simulate timer expiry
    fsm_update(EVENT_TIMEOUT);
    TEST_ASSERT_EQUAL(STATE_LOCKED, current_state);
}

void test_fsm_ignores_invalid_events(void) {
    State start_state = current_state;

    // Sending a 'Door Closed' event while it's already locked
    // shouldn't change anything if no transition is defined.
    fsm_update(EVENT_DOOR_CLOSED);

    TEST_ASSERT_EQUAL(start_state, current_state);
}

void test_fsm_state_to_string(void) {
    TEST_ASSERT_EQUAL_STRING("STATE_LOCKED", fsm_state_to_string(STATE_LOCKED));
    TEST_ASSERT_EQUAL_STRING("STATE_UNLOCKED", fsm_state_to_string(STATE_UNLOCKED));
    TEST_ASSERT_EQUAL_STRING("STATE_ALARM", fsm_state_to_string(STATE_ALARM));
    TEST_ASSERT_EQUAL_STRING("STATE_UNKNOWN", fsm_state_to_string(STATE_UNKNOWN));
    TEST_ASSERT_EQUAL_STRING("STATE_UNKNOWN", fsm_state_to_string(1337));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fsm_valid_pin_unlocks);
    RUN_TEST(test_fsm_breach_triggers_alarm);
    RUN_TEST(test_fsm_timeout_relocks);
    RUN_TEST(test_fsm_ignores_invalid_events);
    RUN_TEST(test_fsm_state_to_string);
    return UNITY_END();
}
