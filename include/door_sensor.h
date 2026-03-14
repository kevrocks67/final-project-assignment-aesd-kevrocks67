/**
 * @file door_sensor.h
 * @brief Interface for the magnetic contact switch attached to the door
 * * This module interprets raw Linux input events from the gpio-keys driver
 * and maintains the logical state (Open/Closed) of the door.
 */

#ifndef DOOR_SENSOR_H
#define DOOR_SENSOR_H

#include <linux/input.h>
#include <stdbool.h>

#define DOOR_OPEN 0
#define DOOR_CLOSED 1

/**
 * @brief Processes a raw Linux input event.
 * * Maps EV_KEY events from the magnetic switch (code 256) to the
 * our boolean that tracks state
 * * @param ev Pointer to the input_event structure provided by the kernel
 */
void door_sensor_process_event(const struct input_event* ev);

/**
 * @brief Checks if the door is currently open.
 * * @return true if the magnet is removed (door open).
 * @return false if the magnet is present (door closed).
 */
bool door_sensor_is_open(void);

/**
 * @brief Forcefully resets the internal sensor state.
 * * Really only useful for testing
 */
void door_sensor_reset(void);

#endif /* DOOR_SENSOR_H */
