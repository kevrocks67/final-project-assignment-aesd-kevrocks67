/**
 * @file lock.h
 * @brief Hardware abstraction layer for servo-controlled door lock.
 * * Provides an interface to control a servo motor via Linux PWM sysfs,
 * mapping angular positions to PWM duty cycles.
 */

#ifndef LOCK_H
#define LOCK_H

#include <stdbool.h>

/** @brief Minimum physical limit of the servo in degrees. */
#define SERVO_MIN_DEGREE 0
/** @brief Maximum physical limit of the servo in degrees. */
#define SERVO_MAX_DEGREE 180

/** @brief Angle at which door is considered unlocked */
#define DOOR_UNLOCKED_ANGLE 0
/** @brief Angle at which door is considered locked */
#define DOOR_LOCKED_ANGLE 90

/** @brief The base sysfs path for the BeagleBone Black PWM chip.
 * Note: This index (pwmchip0) may change if other PWM overlays are loaded. */
#define PWM_CHIP_PATH "/sys/class/pwm/pwmchip0"

/** @brief The sub-directory for the specific PWM channel being used */
#define PWM_SUB_PATH  PWM_CHIP_PATH "/pwm0"

/** @brief The PWM channel to use/export */
#define PWM_CHANNEL "0"

/** @brief Standard servo period of 20ms (50Hz) expressed in nanoseconds. */
#define PWM_PERIOD_NS 20000000

/** @brief Calibrated duty cycle for 0 degrees (True Zero) based on hardware testing. */
#define PWM_DUTY_MIN  570000

/** @brief Calibrated duty cycle for 180 degrees based on hardware testing. */
#define PWM_DUTY_MAX  2490000

/**
 * @brief Initializes the PWM hardware for the servo motor.
 * * Exports the PWM channel via sysfs, sets the base period to 20ms (50Hz),
 * and enables the PWM output.
 * * @return true if initialization succeeded, false if hardware access failed.
 */
bool lock_init(void);

/**
 * @brief Sets the servo to a specific angular position.
 * * Calculates the required duty cycle in nanoseconds and writes it to
 * the PWM sysfs duty_cycle attribute.
 * * @param degrees The target angle (clamped between 0 and 180).
 */
void lock_set_angle(int degrees);

/**
 * @brief Retrieves the last commanded servo angle.
 * * @return The current angle in degrees (0-180).
 */
int lock_get_angle(void);

/**
 * @brief Converts a human-readable angle to a PWM duty cycle.
 * * Uses linear interpolation to map degrees to the nanosecond pulse width
 * required by standard analog servos (typically 1ms to 2ms).
 * * @param degrees The target angle.
 * @return The duty cycle in nanoseconds.
 */
int angle_to_duty_cycle_ns(int degrees);

/**
 * @brief Disables PWM output and releases hardware resources.
 * * Disables the PWM channel and unexports it from the sysfs interface.
 */
void lock_cleanup(void);

#endif //LOCK_H
