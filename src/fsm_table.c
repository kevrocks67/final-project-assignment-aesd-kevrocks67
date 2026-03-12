#include "fsm_table.h"
#include "fsm_handlers.h"

const Transition transition_table[] = {
    /* --- STATE_LOCKED --- */
    {
        .current = STATE_LOCKED, .event = EVENT_PIN_VALID, .guard = guard_is_door_closed,
        .action = do_unlock, .next = STATE_UNLOCKED
    },
    {
        .current = STATE_LOCKED, .event = EVENT_PIN_INVALID, .guard = guard_is_max_attempts,
        .action = do_trigger_alarm, .next = STATE_ALARM
    },
    {
        .current = STATE_LOCKED, .event = EVENT_PIN_INVALID, .guard = NULL,
        .action = do_inc_attempts, .next = STATE_LOCKED
    },
    {
        .current = STATE_LOCKED, .event = EVENT_PIN_CANCEL, .guard = NULL,
        .action = do_reset_buffer, .next = STATE_LOCKED
    },
    {
        .current = STATE_LOCKED, .event = EVENT_DOOR_OPEN, .guard = NULL,
        .action = do_trigger_alarm, .next = STATE_ALARM
    },

    /* --- STATE_UNLOCKED --- */
    {
        .current = STATE_UNLOCKED, .event = EVENT_DOOR_CLOSED, .guard = NULL,
        .action = do_lock, .next = STATE_LOCKED
    },
    {
        .current = STATE_UNLOCKED, .event = EVENT_TIMEOUT, .guard = guard_is_door_closed,
        .action = do_lock, .next = STATE_LOCKED
    },
    {
        .current = STATE_UNLOCKED, .event = EVENT_TIMEOUT, .guard = guard_is_door_open,
        .action = do_retry_timer, .next = STATE_UNLOCKED
    },
    {
        .current = STATE_UNLOCKED, .event = EVENT_PIN_CANCEL, .guard = NULL,
        .action = do_reset_buffer, .next = STATE_UNLOCKED
    },

    /* --- STATE_ALARM --- */
    {
        .current = STATE_ALARM, .event = EVENT_PIN_VALID, .guard = NULL,
        .action = do_silence_reset, .next = STATE_UNLOCKED
    },
    {
        .current = STATE_ALARM, .event = EVENT_PIN_CANCEL, .guard = NULL,
        .action = do_reset_buffer, .next = STATE_ALARM
    }
};

const size_t num_transitions = sizeof(transition_table) / sizeof(Transition);
