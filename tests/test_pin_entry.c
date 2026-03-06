#include "unity.h"
#include "pin_entry.h"

void setUp(void) {
    // We need to reset the internal state of the pin_entry module before each test */
    // We can't call reset_buffer directly because it's static in .c
    // But we can trigger it by sending a CANCEL command.
    KeyEvent cancel_ev = { .cmd = KEY_CMD_CANCEL };
    pin_entry_process_key(cancel_ev);
}

void tearDown(void) {
}

/**
 * Helper to simulate pressing a sequence of numbers
 */
void simulate_keypresses(const int* digits, int count) {
    for (int i = 0; i < count; i++) {
        KeyEvent ev = {
            .cmd = KEY_CMD_NUMBER,
            .data.key_number = digits[i]
        };
        pin_entry_process_key(ev);
    }
}

void test_PinEntry_AddSingleDigit(void) {
    int digits[] = {1};
    simulate_keypresses(digits, 1);

    TEST_ASSERT_EQUAL_INT(1, pin_entry_get_count());
}

void test_PinEntry_BufferOverflowPrevention(void) {
    // Try to enter 10 digits when MAX is 8
    int digits[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    simulate_keypresses(digits, 10);

    // Should stop at MAX_PIN_LEN
    TEST_ASSERT_EQUAL_INT(8, pin_entry_get_count());
}

void test_PinEntry_CancelClearsBuffer(void) {
    int digits[] = {1, 2, 3};
    simulate_keypresses(digits, 3);
    TEST_ASSERT_EQUAL_INT(3, pin_entry_get_count());

    KeyEvent cancel_ev = { .cmd = KEY_CMD_CANCEL };
    pin_entry_process_key(cancel_ev);

    TEST_ASSERT_EQUAL_INT(0, pin_entry_get_count());
}

void test_PinEntry_SubmitResetsBuffer(void) {
    int digits[] = {1, 2, 3, 4};
    simulate_keypresses(digits, 4);

    KeyEvent submit_ev = { .cmd = KEY_CMD_SUBMIT };
    pin_entry_process_key(submit_ev);

    // Buffer should be empty after submission
    TEST_ASSERT_EQUAL_INT(0, pin_entry_get_count());
}

void test_PinEntry_IgnoreNoneCommand(void) {
    KeyEvent none_ev = { .cmd = KEY_CMD_NONE };
    pin_entry_process_key(none_ev);

    TEST_ASSERT_EQUAL_INT(0, pin_entry_get_count());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_PinEntry_AddSingleDigit);
    RUN_TEST(test_PinEntry_BufferOverflowPrevention);
    RUN_TEST(test_PinEntry_CancelClearsBuffer);
    RUN_TEST(test_PinEntry_SubmitResetsBuffer);
    RUN_TEST(test_PinEntry_IgnoreNoneCommand);
    return UNITY_END();
}
