/**
 * @file door_sensor.c
 * @brief Implementation of the magnetic sensor watcher logic.
 * * Handles state transitions and provides hooks for
 * state machine integration.
 */
#include "door_sensor.h"
#include <sys/syslog.h>
#include <stddef.h>

/** @brief Internal tracker for the door's physical state. */
static bool is_door_open = false;

void door_sensor_process_event(struct input_event* ev) {
    if (ev == NULL) {
        return;
    }

    // Check for state changes
    if (ev->type == EV_KEY && ev->code == 256) {
        bool is_door_open_currently = (ev->value == 0);

        if (is_door_open_currently != is_door_open) {
            is_door_open = is_door_open_currently;
            syslog(LOG_INFO, "Door state changed to %s",
                   is_door_open ? "Opened" : "Closed");
            // Update state machine
        }

    }
}

bool door_sensor_is_open() {
    return is_door_open;
}

void door_sensor_reset() {
    is_door_open = false;
}
