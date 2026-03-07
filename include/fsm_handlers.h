/**
 * @file fsm_handlers.h
 * @brief Function prototypes for FSM guards and actions.
 * * This header defines the "Menu" of available logic gates (guards) and
 * hardware side-effects (actions) used by the transition table.
 * Implementation details for these handlers are located in fsm_handlers.c.
 */
#ifndef FSM_HANDLERS_H
#define FSM_HANDLERS_H

#include "fsm_types.h"

/**
 * @brief Commands the servo motor to move to the UNLOCKED position (0°).
 */
void do_unlock(void);

/**
 * @brief Commands the servo motor to move to the LOCKED position (90°).
 */
void do_lock(void);

/**
 * @brief Activates the security siren and visual alarm indicators.
 */
void do_trigger_alarm(void);

/**
 * @brief Increments the internal counter for failed PIN entries.
 */
void do_inc_attempts(void);

/**
 * @brief Clears the current PIN input buffer and resets related UI elements.
 */
void do_reset_buffer(void);

/**
 * @brief Restarts the software auto-lock timer.
 */
void do_retry_timer(void);

/**
 * @brief Deactivates the alarm and resets the failed attempts counter to zero.
 */
void do_silence_reset(void);

/**
 * @brief Displays a "Please Close Door" notification to the user.
 */
void do_notify_close(void);

/**
 * @brief Checks if the door is physically closed via the magnetic sensor.
 * @return true if sensor indicates CLOSED, false otherwise.
 */
bool guard_is_door_closed(void);

/**
 * @brief Checks if the door is physically open.
 * @return true if sensor indicates OPEN, false otherwise.
 */
bool guard_is_door_open(void);

/**
 * @brief Determines if the consecutive failed PIN attempts have reached the limit.
 * @return true if attempts >= MAX_ATTEMPTS, false otherwise.
 */
bool guard_is_max_attempts(void);

/**
 * @brief Determines if the consecutive failed PIN attempts have reached the limit.
 * @return true if attempts >= MAX_ATTEMPTS, false otherwise.
 */
bool guard_is_not_max_attempts(void);

#endif //FSM_HANDLERS_H
