/**
* @file keypad_parser.h
* @brief Public API for translating Linux input events to system commands.
*
* This header defines the KeyEvent structure and the parsing function used
* by the door security system to interpret keypad input from the Linux
* input subsystem.
*/

#ifndef KEYPAD_PARSER_H
#define KEYPAD_PARSER_H

#include <linux/input.h>

typedef enum {
    KEY_CMD_NONE = 0,  /**< No valid key press detected or event ignored. */
    KEY_CMD_NUMBER,    /**< A numeric digit (0-9) was pressed. */
    KEY_CMD_SUBMIT,    /**< The EOP (End Of Pin) symbol, typically the # key. */
    KEY_CMD_CANCEL     /**< The cancellation symbol, typically the * key. */
} KeyCommand;

typedef struct {
    KeyCommand cmd;      /**< The type of command mapped from the input. */
    union {
        int key_number;  /**< Numeric value (0-9) associated with KEY_CMD_NUMBER. */
    } data;              /**< Associated data payload for the command. */
} KeyEvent;

/**
 * @brief Translates a Linux input_event into a system-specific KeyEvent.
 * * Filters events based on type (EV_KEY) and value (1 for press).
 * Events that do not match these criteria, or keys not defined in the
 * switch table, return a KeyEvent with cmd set to KEY_CMD_NONE.
 * * @param ev Pointer to the raw input_event struct read from the device.
 * @return KeyEvent The translated command and associated data.
 */
KeyEvent process_keypad_event(const struct input_event* ev);

#endif //KEYPAD_PARSER_H
