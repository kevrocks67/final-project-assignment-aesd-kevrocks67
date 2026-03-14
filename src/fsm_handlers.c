#include "fsm_handlers.h"
#include "fsm.h"
#include "door_sensor.h"
#include "lock.h"
#include <syslog.h>
#include <sys/timerfd.h>

void do_unlock() {
    lock_set_angle(DOOR_UNLOCKED_ANGLE);
    syslog(LOG_USER | LOG_INFO, "Unlocking door");
    do_start_timer();
}

void do_lock() {
    lock_set_angle(DOOR_LOCKED_ANGLE);
    syslog(LOG_USER | LOG_INFO, "Locking door");
}

void do_trigger_alarm() {
    syslog(LOG_CRIT, "ALARM TRIGGERED: Unauthorized Access Attempt");
}

void do_inc_attempts() {
    fsm_inc_attempts();
    syslog(LOG_USER | LOG_INFO, "Failed PIN attempt incremented. Attempts: %d", fsm_get_attempts());
}

void do_reset_buffer() {
    // Note: Buffer is already cleared by pin_entry when KEY_CMD_CANCEL is processed.
    // This is a no-op in the FSM layer since PIN management belongs to the pin_entry module.
}

void do_start_timer() {
    int timer_fd = fsm_get_timer_fd();
    struct itimerspec spec = {
        .it_interval = {0, 0},
        .it_value = fsm_get_unlock_timeout()
    };

    if (timerfd_settime(timer_fd, 0, &spec, NULL) == -1) {
        syslog(LOG_ERR, "Failed to start unlock timer");
    } else {
        syslog(LOG_USER | LOG_DEBUG, "Unlock timer started");
    }
}

void do_retry_timer() {
    do_start_timer();
}

void do_silence_reset() {
    fsm_reset_attempts();
    do_unlock();
    do_retry_timer();
    syslog(LOG_CRIT, "ALARM SILENCED: Valid PIN entered, unlocking door for user to close");
}

void do_notify_close() {
    syslog(LOG_USER | LOG_INFO, "Please close the door");
}

bool guard_is_door_closed() {
    return !door_sensor_is_open();
}

bool guard_is_door_open() {
    return door_sensor_is_open();
}

static bool check_max_attempts() {
    return fsm_get_attempts() >= MAX_ATTEMPTS;
}

bool guard_is_max_attempts() {
    return check_max_attempts();
}

bool guard_is_not_max_attempts() {
    return !check_max_attempts();
}
