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
 * @enum PinStatus
 * @brief Represents the result of a key processing event in the PIN entry logic.
 * * This status allows the caller (usually the main event loop) to determine
 * if the FSM needs to be updated based on user input.
 */
typedef enum {
    PIN_STATUS_IN_PROGRESS, /**< Character added, but PIN is not yet complete. */
    PIN_STATUS_VALID,       /**< User submitted a PIN and it matched the MASTER_PIN. */
    PIN_STATUS_INVALID,     /**< User submitted a PIN but it did not match. */
    PIN_STATUS_CANCELLED    /**< User pressed the cancel key; buffer has been cleared. */
} PinStatus;

/**
 * @brief Processes a raw key event and updates the internal PIN buffer.
 * * This function manages the state of the PIN entry buffer. It appends numbers,
 * handles cancellations, and performs a string comparison against the master PIN
 * when a submit command is received. The buffer is automatically reset on
 * SUBMIT or CANCEL.
 * * @param event The raw KeyEvent received from the keypad driver/parser.
 * @return PinStatus The resulting status of the PIN entry after this keypress.
 * * @note This function logs attempts to syslog and handles the transition from
 * raw hardware events to high-level logic statuses.
 */
PinStatus pin_entry_process_key(KeyEvent event);

/**
 * @brief Retrieves the number of digits currently stored in the buffer.
 * * Mainly used for unit testing to verify that the buffer is filling
 * and clearing correctly.
 * * @return int The number of digits (0 to MAX_PIN_LEN).
 */
int pin_entry_get_count(void);


#endif //PIN_ENTRY_H
