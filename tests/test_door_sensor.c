#include "unity.h"
#include "door_sensor.h"
#include <linux/input.h>

void setUp(void) {
    //Reset the sensor state before every test
    door_sensor_reset();
}

void tearDown(void) {
}

//Test that the sensor defaults to closed (false) on startup.
void test_door_sensor_initial_state(void) {
    TEST_ASSERT_FALSE_MESSAGE(door_sensor_is_open(), "Door should be closed by default");
}

void test_door_sensor_opens_on_valid_event(void) {
    struct input_event ev;
    ev.type = EV_KEY;
    ev.code = 256;
    ev.value = 0;

    door_sensor_process_event(&ev);
    TEST_ASSERT_TRUE_MESSAGE(door_sensor_is_open(), "Door should be open after receiving value 0");
}

void test_door_sensor_ignores_wrong_code(void) {
    struct input_event ev;
    ev.type = EV_KEY;
    ev.code = 100;
    ev.value = 0;

    door_sensor_process_event(&ev);
    TEST_ASSERT_FALSE_MESSAGE(door_sensor_is_open(), "Door should remain closed if code is not 256");
}

void test_door_sensor_ignores_non_key_events(void) {
    struct input_event ev;
    ev.type = EV_REL; // Relative movement event
    ev.code = 256;
    ev.value = 0;

    door_sensor_process_event(&ev);
    TEST_ASSERT_FALSE_MESSAGE(door_sensor_is_open(), "Door should remain closed for non-EV_KEY types");
}

// Test that redundant events do not flip the state incorrectly.
void test_door_sensor_redundant_events(void) {
    struct input_event ev;
    ev.type = EV_KEY;
    ev.code = 256;
    ev.value = 0; // Open

    door_sensor_process_event(&ev);
    TEST_ASSERT_TRUE(door_sensor_is_open());

    // Send the same "open" event again
    door_sensor_process_event(&ev);
    TEST_ASSERT_TRUE_MESSAGE(door_sensor_is_open(), "Door should stay open on redundant events");
}

void test_door_sensor_full_cycle(void) {
    struct input_event ev;
    ev.type = EV_KEY;
    ev.code = 256;

    // Transition to Open
    ev.value = 0;
    door_sensor_process_event(&ev);
    TEST_ASSERT_TRUE(door_sensor_is_open());

    // Transition to Closed
    ev.value = 1;
    door_sensor_process_event(&ev);
    TEST_ASSERT_FALSE_MESSAGE(door_sensor_is_open(), "Door should be closed after value 1");
}

void test_door_sensor_null_event(void) {
    door_sensor_process_event(NULL);
    TEST_ASSERT_FALSE(door_sensor_is_open());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_door_sensor_initial_state);
    RUN_TEST(test_door_sensor_opens_on_valid_event);
    RUN_TEST(test_door_sensor_ignores_wrong_code);
    RUN_TEST(test_door_sensor_ignores_non_key_events);
    RUN_TEST(test_door_sensor_redundant_events);
    RUN_TEST(test_door_sensor_full_cycle);
    RUN_TEST(test_door_sensor_null_event);
    return UNITY_END();
}
