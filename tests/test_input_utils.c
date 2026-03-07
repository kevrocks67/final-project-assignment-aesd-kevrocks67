#include "unity.h"
#include "input_utils.h"

#define MOCK_FILE_PATH "../tests/mocks/mock_devices.txt"

void setUp(void) {
    input_utils_set_path(MOCK_FILE_PATH);
}

void tearDown(void) {
}

void test_find_input_event_num_success(void) {
    int event = find_input_event_num("Keypad");
    TEST_ASSERT_EQUAL_INT(2, event);

    event = find_input_event_num("Sensor");
    TEST_ASSERT_EQUAL_INT(5, event);
}

void test_find_input_event_num_missing_event_tag(void) {
    // In mock, NoEventDevice has Handlers=kbd mouse
    int event = find_input_event_num("NoEventDevice");
    TEST_ASSERT_EQUAL_INT(-1, event);
}

void test_find_input_event_num_empty_event_number(void) {
    // In mock, EmptyEventDevice has Handlers=kbd event
    // Our logic 'event_tok_ptr_len > event_tok_len' should catch this
    int event = find_input_event_num("EmptyEventDevice");
    TEST_ASSERT_EQUAL_INT(-1, event);
}

void test_find_input_event_num_device_not_found(void) {
    int event = find_input_event_num("ImaginaryKeyboard");
    TEST_ASSERT_EQUAL_INT(-1, event);
}

void test_find_input_event_num_file_error(void) {
    input_utils_set_path("non_existent_file.txt");
    int event = find_input_event_num("Keypad");
    TEST_ASSERT_EQUAL_INT(-2, event);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_find_input_event_num_success);
    RUN_TEST(test_find_input_event_num_missing_event_tag);
    RUN_TEST(test_find_input_event_num_empty_event_number);
    RUN_TEST(test_find_input_event_num_device_not_found);
    RUN_TEST(test_find_input_event_num_file_error);
    return UNITY_END();
}
