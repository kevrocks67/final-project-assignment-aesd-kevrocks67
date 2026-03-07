#include "fsm.h"
#include <stddef.h>

State current_state;

int fsm_init() {
    current_state = STATE_LOCKED;
    return 0;
}

void fsm_update(Event event) {
}

const char* fsm_state_to_string(int state) {
    const char* state_str;

    switch (state) {
        case STATE_LOCKED:
            state_str = "STATE_LOCKED";
            break;
        case STATE_UNLOCKED:
            state_str = "STATE_UNLOCKED";
            break;
        case STATE_ALARM:
            state_str = "STATE_ALARM";
            break;
        default:
            state_str = "STATE_UNKNOWN";
            break;
    }
    return state_str;
}
