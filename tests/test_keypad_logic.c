#include "unity.h"
#include "keypad_logic.h"
#include <linux/input.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/**
 * Helper to create a mock input event
 */
static struct input_event create_press_event(int code) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.value = 1; // 1 = Press, 0 = Release
    ev.code = code;
    return ev;
}

void test_valid_digits(void) {
    // Test Key '1'
    struct input_event ev = create_press_event(KEY_1);
    KeyEvent result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_NUMBER, result.cmd);
    TEST_ASSERT_EQUAL_INT(1, result.data.key_number); // Accessing the union

    // Test Key '0'
    ev = create_press_event(KEY_0);
    result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_NUMBER, result.cmd);
    TEST_ASSERT_EQUAL_INT(0, result.data.key_number);
}

void test_special_keys(void) {
    // Test Submit (#/Enter)
    struct input_event ev = create_press_event(KEY_ENTER);
    KeyEvent result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_SUBMIT, result.cmd);

    // Test Cancel (*/Asterisk)
    ev = create_press_event(KEY_KPASTERISK);
    result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_CANCEL, result.cmd);
}

void test_ignore_non_key_events(void) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));

    // Ignore relative motion (EV_REL = 2)
    ev.type = 2;
    ev.value = 1;
    KeyEvent result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_NONE, result.cmd);

    // Ignore key releases (value 0)
    ev.type = EV_KEY;
    ev.value = 0;
    ev.code = KEY_1;
    result = process_keypad_event(&ev);
    TEST_ASSERT_EQUAL_INT(KEY_CMD_NONE, result.cmd);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_digits);
    RUN_TEST(test_special_keys);
    RUN_TEST(test_ignore_non_key_events);
    return UNITY_END();
}
