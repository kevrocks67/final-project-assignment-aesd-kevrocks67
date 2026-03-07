/**
 * @file fsm_types.h
 * @brief Types definitions and structures for the FSM
*/
#ifndef FSM_TYPES_H
#define FSM_TYPES_H

#include <stdbool.h>

/**
 * @brief System security states.
 */
typedef enum {
    STATE_UNKNOWN = 0, /**< State is unknown. */
    STATE_LOCKED,      /**< Door is secured; Servo at 90 degrees. */
    STATE_UNLOCKED,    /**< Access granted; Servo at 0 degrees. */
    STATE_ALARM        /**< Security breach detected; Door forced open */
} State;

/**
 * @brief System events that trigger state transitions.
 * @note These are inputs to the FSM. Actions (like moving the servo) are outputs.
 */
typedef enum {
    EVENT_PIN_VALID = 0,  /**< Valid PIN entered and submitted. */
    EVENT_PIN_INVALID,    /**< Invalid PIN submitted. */
    EVENT_PIN_CANCEL,     /**< User cancelled PIN entry. */
    EVENT_DOOR_OPEN,      /**< Magnetic sensor detects door is open. */
    EVENT_DOOR_CLOSED,    /**< Magnetic sensor detects door is closed. */
    EVENT_TIMEOUT         /**< Relock timer has expired. */
} Event;

/** * @brief Function pointer for FSM state-change side effects.
 * @note Function definitions in fsm_handlers.c; used for outputs
 */
typedef void (*fsm_action_t)(void);

/** * @brief Function pointer for FSM transition logic gates.
 * @return true if the transition criteria are met, false to stay in current state.
 * @note Implemented in fsm_handlers.c to control hardware (Servo, LEDs, Buzzer).
 */
typedef bool (*fsm_guard_t)(void);

/**
 * @brief Defines a single row in the Finite State Machine's transition table.
 * * Each Transition represents a rule: "If the system is in @p current state
 * and @p event occurs, check @p guard. If true, execute @p action and
 * move to @p next state."
 */
typedef struct {
    /** @brief The required starting state for this transition to be valid. */
    State current;

    /** @brief The trigger that initiates the transition evaluation. */
    Event event;

    /** @brief The destination state after the transition completes. */
    State next;

    /** @brief Optional condition check before the transition fires.*/
    fsm_guard_t guard;

    /** @brief Optional side-effect executed during the state change. */
    fsm_action_t action;
} Transition;

#endif //FSM_TYPES_H
