#ifndef KEYPAD_LOGIC_H
#define KEYPAD_LOGIC_H

#include <linux/input.h>

typedef enum {
    KEY_CMD_NONE = 0,
    KEY_CMD_NUMBER,   // Digits 0-9
    KEY_CMD_SUBMIT,   // The # key
    KEY_CMD_CANCEL    // The * key
} KeyCommand;

typedef struct {
    KeyCommand cmd;
    union {
        int key_number;        // Will hold 0-9 if cmd is KEY_CMD_NUMBER
    } data;
} KeyEvent;

/**
 * Parses a raw Linux input_event and maps it to our internal logic.
 * Returns a KeyEvent with KEY_CMD_NONE if the event is ignored.
 */
KeyEvent process_keypad_event(const struct input_event* ev);

#endif
