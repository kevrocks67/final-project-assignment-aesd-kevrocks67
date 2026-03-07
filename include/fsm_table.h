/**
 * @file fsm_table.h
 * @brief External interface for the FSM transition table.
 */

#ifndef FSM_TABLE_H
#define FSM_TABLE_H

#include "fsm_types.h"
#include <stddef.h>

/**
 * @brief The Functional Transition Table
 * Only contains transitions that result in a state change or an output action.
 */
extern const Transition transition_table[];

/**
 * @brief The number of rows in the transition tabkle
 */
extern const size_t num_transitions;

#endif //FSM_TABLE_H
