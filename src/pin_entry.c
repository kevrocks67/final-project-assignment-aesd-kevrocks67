#include "pin_entry.h"
#include <string.h>
#include <sys/syslog.h>

// Internal state hidden from other files (static)
static char pin_buffer[MAX_PIN_LEN + 1];
static int current_len = 0;

// TODO: Temporary until this is moved somewhere else and hashed
const char* MASTER_PIN = "1234";

/**
 * Reset the internal buffer and index.
 * Useful for both 'Cancel' and after a successful 'Submit'.
 */
static void reset_buffer(void) {
    memset(pin_buffer, '\0', sizeof(pin_buffer));
    current_len = 0;
}

static void check_pin() {
    if(strncmp(pin_buffer, MASTER_PIN, MAX_PIN_LEN) == 0) {
        syslog(LOG_USER | LOG_INFO, "Pin is correct, unlocking door");
    } else {
        syslog(LOG_USER | LOG_INFO, "Pin is incorrect");
    }
}


void pin_entry_process_key(KeyEvent event) {
    switch (event.cmd) {
        case KEY_CMD_NUMBER:
            if (current_len < MAX_PIN_LEN) {
                syslog(LOG_USER | LOG_DEBUG, "Pin submitted: %s", pin_buffer);
                pin_buffer[current_len] = (char) (event.data.key_number + '0');
                current_len++;
                pin_buffer[current_len] = '\0';
            } else {
                syslog(LOG_USER | LOG_DEBUG, "Pin buffer is full: %d/%d", current_len, MAX_PIN_LEN);
            }
            break;

        case KEY_CMD_SUBMIT:
            if (current_len > 0) {
                syslog(LOG_USER | LOG_INFO, "Pin submitted: %s", pin_buffer);
                check_pin();
            }
            reset_buffer();
            break;

        case KEY_CMD_CANCEL:
            syslog(LOG_USER | LOG_INFO, "Pin entry cancelled. Clearing buffer");
            reset_buffer();
            break;

        default:
            // Ignore KEY_CMD_NONE or unexpected events
            break;
    }
}

int pin_entry_get_count() {
    return current_len;
}
