/**
 * @file fsm.h
 * @brief Finite State Machine for the Door Security System.
 * * This module implements a Mealy machine that coordinates the Keypad PIN entry,
 * the Servo lock, and the Magnetic Door Sensor.
 */
#ifndef FSM_H
#define FSM_H

#include "fsm_types.h"

/**
 * @brief Initializes the FSM to its starting state (LOCKED).
 * * Also responsible for initializing the timerfd used for auto-relocking.
 * @return 0 on success, -1 on failure.
 */
int fsm_init(void);

/**
 * @brief The event processor for the FSM.
 * * Takes an event, checks the current state based on inputs to the system,
 * executes the required Mealy actions (outputs) and updates the state.
 * * @param event The triggered event to process.
 */
void fsm_update(Event event);

/**
 * @brief Stringify State types
 * @return String representation of the current state.
 */
const char* fsm_state_to_string(int state);

#endif //FSM_H
