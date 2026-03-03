/**
 * @file pin_entry.h
 * @brief Logic for collecting and buffering keypad digits into a PIN.
 * * This module manages an internal buffer to store up to 8 digits and
 * handles the logic for submitting or canceling the entry.
 */
#ifndef PIN_ENTRY_H
#define PIN_ENTRY_H

#include "keypad_parser.h"

/** * @brief The maximum number of digits allowed for a PIN entry.
 */
#define MAX_PIN_LEN 8

/**
 * @brief Processes a structured key event.
 * * Depending on the command in the KeyEvent, this function will:
 * - Append a digit to the buffer (if under the 8-character limit).
 * - Print the current buffer to the console (for debug/DoD).
 * - Print the final PIN and reset upon a SUBMIT command.
 * - Clear the buffer upon a CANCEL command.
 * * @param event The KeyEvent received from the keypad parser.
 */
void pin_entry_process_key(KeyEvent event);

/**
 * @brief Retrieves the number of digits currently stored in the buffer.
 * * Mainly used for unit testing to verify that the buffer is filling
 * and clearing correctly.
 * * @return int The number of digits (0 to MAX_PIN_LEN).
 */
int pin_entry_get_count(void);


#endif //PIN_ENTRY_H
