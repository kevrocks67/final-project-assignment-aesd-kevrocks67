#include "fsm.h"
#include "fsm_table.h"
#include <stddef.h>
#include <stdio.h>
#include <time.h>

static FSM door_security_fsm;

int fsm_init(int timer_fd, time_t timeout_sec) {
    door_security_fsm.current_state = STATE_LOCKED;
    door_security_fsm.failed_attempts = 0;
    door_security_fsm.timer_fd = timer_fd;
    door_security_fsm.unlock_timeout.tv_sec = timeout_sec;
    door_security_fsm.unlock_timeout.tv_nsec = 0;
    return 0;
}

void fsm_update(Event event) {
    for (size_t i = 0; i < num_transitions; i++) {
        const Transition* t = &transition_table[i];

        if (t->current == door_security_fsm.current_state && t->event == event) {
            if (t->guard == NULL || t->guard()) {
                if (t->action != NULL) {
                    t->action();
                }
                printf("Failed Attempts: %d, Current State: %d, Event: %d\n, Next State: %d\n", door_security_fsm.failed_attempts, door_security_fsm.current_state, event, t->next);
                door_security_fsm.current_state = t->next;
                printf("Current State: %d, Event: %d\n", door_security_fsm.current_state, event);
                break;
            }
        }
    }
}

State fsm_get_state() {
    return door_security_fsm.current_state;
}

int fsm_get_timer_fd() {
    return door_security_fsm.timer_fd;
}

const int fsm_get_attempts() {
    return door_security_fsm.failed_attempts;
}

void fsm_inc_attempts() {
    door_security_fsm.failed_attempts++;
}

void fsm_reset_attempts() {
    door_security_fsm.failed_attempts = 0;
}

struct timespec fsm_get_unlock_timeout() {
    return door_security_fsm.unlock_timeout;
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
