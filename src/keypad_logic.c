#include "keypad_logic.h"
#include <linux/input.h>

KeyEvent process_keypad_event(const struct input_event* ev) {
    KeyEvent result = { .cmd = KEY_CMD_NONE, .data.key_number = 0 };
    (void)ev;
    return result;
}
