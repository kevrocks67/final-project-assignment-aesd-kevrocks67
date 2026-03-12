/**
 * @file fsm.h
 * @brief Finite State Machine for the Door Security System.
 * * This module implements a Mealy machine that coordinates the Keypad PIN entry,
 * the Servo lock, and the Magnetic Door Sensor.
 */
#ifndef FSM_H
#define FSM_H

#include "fsm_types.h"
#include <stdint.h>
#include <time.h>

static const int MAX_ATTEMPTS = 3;

/**
 * @brief Persistent context for the security state machine.
 * * This structure stores the "memory" of the system, including the current
 * state, security counters, and hardware timer configurations.
 */
typedef struct {
    /** @brief The current operational state (LOCKED, UNLOCKED, ALARM). */
    State current_state;

    /** @brief Tracks consecutive invalid PIN entries for lockout logic. */
    int failed_attempts;

    /** @brief The file descriptor for the Linux timerfd. */
    int timer_fd;

    /** @brief Configurable timeout setting for automatic relock. */
    struct timespec unlock_timeout;
} FSM;

/**
 * @brief Initializes the FSM context.
 * @param timer_fd The file descriptor for the system timer.
 * @param timeout_sec The default timeout duration in seconds.
 * @return 0 on success, non-zero on failure.
 * * Sets the initial state to @ref STATE_LOCKED and resets counters.
 */
int fsm_init(int timer_fd, time_t timeout_sec);

/**
 * @brief The event processor for the FSM.
 * * Takes an event, checks the current state based on inputs to the system,
 * executes the required Mealy actions (outputs) and updates the state.
 * * @param event The triggered event to process.
 */
void fsm_update(Event event);

/**
 * @brief Retrieves the current state of the machine.
 * @return The current @ref State.
 */
State fsm_get_state(void);

/**
 * @brief Retrieves the timer file descriptor.
 * @return The timerfd integer.
 */
int fsm_get_timer_fd(void);

/**
 * @brief Retrieves the configured timeout duration.
 * @return The timeout as a struct timespec.
 */
struct timespec fsm_get_unlock_timeout(void);

/**
 * @brief Retrieves the number of failed attempts
 * @return The number of failed attempts
 */
const int fsm_get_attempts(void);

/**
 *  @brief Increases the number of Pin Entry failed attempts by 1
 */
void fsm_inc_attempts(void);

/**
 *  @brief Resets the number of Pin Entry failed attempts to 0
 */
void fsm_reset_attempts(void);

/**
 * @brief Converts a state to a human-readable string.
 * @param state The state integer to convert.
 * @return A constant string representing the state name.
 */
const char* fsm_state_to_string(int state);

#endif //FSM_H
