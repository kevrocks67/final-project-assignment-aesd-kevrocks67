#include "lock.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>

static int current_angle = 0;

static bool write_sysfs(const char* path, const char* value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return false;
    }

    ssize_t bytes = write(fd, value, strlen(value));

    close(fd);
    return bytes > 0;
}

bool lock_init() {
    char buf[16];

    // Export the PWM device
    if (!write_sysfs(PWM_CHIP_PATH "/export", PWM_CHANNEL)) {
        syslog(LOG_DEBUG, "Lock: pwm0 already exported or chip not found");
    }

    // Wait arbitrary time for export
    usleep(250000);

    // Set period to 20ms (50 hz freq)
    snprintf(buf, sizeof(buf), "%d", PWM_PERIOD_NS);
    if (!write_sysfs(PWM_SUB_PATH "/period", buf)) {
        syslog(LOG_ERR, "Lock: Failed to set period at %s", PWM_SUB_PATH);
        return false;
    }

    // Set initial state to locked
    int initial_duty = angle_to_duty_cycle_ns(0);
    snprintf(buf, sizeof(buf), "%d", initial_duty);
    write_sysfs(PWM_SUB_PATH "/duty_cycle", buf);

    // Enable the PWM device
    write_sysfs(PWM_SUB_PATH "/enable", "1");

    current_angle = 0;
    syslog(LOG_INFO, "Lock: ehrpwm1 initialized successfully");
    return true;
}

void lock_set_angle(int degrees) {
    char buf[16];
    int duty_ns = angle_to_duty_cycle_ns(degrees);

    snprintf(buf, sizeof(buf), "%d", duty_ns);
    if (write_sysfs(PWM_SUB_PATH "/duty_cycle", buf)) {
        current_angle = degrees;
        syslog(LOG_DEBUG, "Lock: Angle set to %d", degrees);
    }
}

int lock_get_angle() {
    return current_angle;
}

int angle_to_duty_cycle_ns(int degrees) {
    /* Clamp input to physical hardware limits */
    if (degrees < SERVO_MIN_DEGREE) {
        degrees = SERVO_MIN_DEGREE;
    }

    if (degrees > SERVO_MAX_DEGREE) {
        degrees = SERVO_MAX_DEGREE;
    }

    return PWM_DUTY_MIN + (degrees * (PWM_DUTY_MAX - PWM_DUTY_MIN) / 180);
}

void lock_cleanup() {
    write_sysfs(PWM_SUB_PATH "/enable", "0");
    write_sysfs(PWM_CHIP_PATH "/unexport", "0");
}
