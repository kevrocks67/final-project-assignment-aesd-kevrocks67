#include "unity.h"
#include "lock.h"


void setUp(void) {
    /* This runs BEFORE each test */
}

void tearDown(void) {
    /* This runs AFTER each test */
}

void test_is_locked(void) {
    TEST_ASSERT_TRUE(is_locked());
}
