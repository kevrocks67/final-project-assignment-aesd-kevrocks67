/**
 * @file main.c
 * @brief Main entry point for the Door Security Daemon.
 *
 * This module implements the top-level epoll event loop that orchestrates
 * inputs from the keypad, door sensor, and system timers. It translates
 * hardware interrupts and signals into FSM events to control the door lock.
 * * @author Kevin Diaz
 * @date March 2026
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "door_sensor.h"
#include "pin_entry.h"
#include "keypad_parser.h"
#include "input_utils.h"
#include "lock.h"
#include "fsm.h"

/** @brief Maximum number of events epoll can handle per wakeup. */
#define MAX_EVENTS 10
/** @brief Number of seconds to wait before re-locking the lock */
#define UNLOCK_TIMEOUT_SECS 15

/**
 * @struct DaemonContext
 * @brief Holds file descriptors for all system components.
 * * This structure centralizes resources to simplify function signatures
 * and ensure clean resource management during teardown.
 */
typedef struct {
    int epoll_fd;       /**< Epoll instance file descriptor. */
    int sig_fd;         /**< Signal file descriptor for SIGINT/SIGTERM. */
    int timer_fd;       /**< Timerfd used for auto-relock logic. */
    int keypad_fd;      /**< Input device descriptor for the matrix keypad. */
    int door_sensor_fd; /**< Input device descriptor for the door sensor. */
} DaemonContext;

/**
 * @brief Dynamically finds and opens an input device.
 * * Uses the input_utils library to map a device name to an event number.
 * * @param name The "Name=" string as it appears in /proc/bus/input/devices.
 * @return int File descriptor on success, -1 on failure.
 */
static int open_input_device(const char* name) {
    int event_num = find_input_event_num(name);
    if (event_num < 0) {
        syslog(LOG_ERR, "Could not find event number for: %s", name);
        return -1;
    }
    char path[32];
    snprintf(path, sizeof(path), "/dev/input/event%d", event_num);
    return open(path, O_RDONLY | O_NONBLOCK);
}

/**
 * @brief Initializes all hardware, signals, and FSM.
 * * Opens descriptors to our hardware, initializes the FSM
 *   and configures the relock timer.
 * * @param ctx Pointer to the DaemonContext to be populated.
 * @return int 0 on success, -1 on critical initialization failure.
 */
static int initialize_system(DaemonContext* ctx) {
    // Initialize lock
    if (!lock_init()) {
        syslog(LOG_ERR, "Failed to initialize PWM lock hardware.");
        return -1;
    }

    // Open descriptors to our keypad and door sensor
    int keypad_fd = open_input_device("matrix_keypad");
    int door_sensor_fd = open_input_device("door_sensor");

    if (keypad_fd < 0 || door_sensor_fd < 0) {
        syslog(LOG_ERR, "Critical: Keypad or Door Sensor not found.");
        lock_cleanup();
        return -1;
    }
    ctx->keypad_fd = keypad_fd;
    ctx->door_sensor_fd = door_sensor_fd;

    // Setup SIGINT and SIGTERM handlers
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);

    // Block signals from their default handlers so they can be read via signalfd
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        syslog(LOG_ERR, "Sigprocmask failed: %m");
        return -1;
    }

    // Create the signalfd
    int sig_fd = signalfd(-1, &mask, 0);
    if (sig_fd == -1) {
        syslog(LOG_ERR, "Signalfd creation failed: %m");
        return -1;
    }
    ctx->sig_fd = sig_fd;

    // Create the timer
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1) {
        syslog(LOG_ERR, "Timerfd creation failed: %m");
        return -1;
    }
    ctx->timer_fd = timer_fd;

    // Initialize FSM
    fsm_init(timer_fd, UNLOCK_TIMEOUT_SECS);

    return 0;
}

/**
 * @brief Configures the epoll instance to watch system file descriptors.
 * * @param ctx Pointer to context containing valid file descriptors.
 * @return int 0 on success, -1 on epoll configuration failure.
 */
static int setup_event_polling(DaemonContext* ctx) {
    int epoll_fd = epoll_create1(O_CLOEXEC);
    if (epoll_fd == -1) {
        syslog(LOG_ERR, "epoll_create1 failed: %m");
        return -1;
    }
    ctx->epoll_fd = epoll_fd;

    struct epoll_event ev;
    int fds[] = {
        ctx->sig_fd, ctx->timer_fd,
        ctx->keypad_fd, ctx->door_sensor_fd
    };

    for (int i = 0; i < (sizeof(fds) / sizeof(fds[0])); i++) {
        ev.events = EPOLLIN;
        ev.data.fd = fds[i];
        if (epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
            syslog(LOG_ERR, "epoll_ctl failed to add fd: %m");
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Processes individual events triggered by epoll.
 * * Routes hardware inputs to the appropriate driver and updates the FSM.
 * * @param fd The file descriptor that triggered the event.
 * @param ctx Pointer to the system context.
 * @param running Pointer to the loop control flag.
 */
static void dispatch_event(int fd, DaemonContext* ctx, int* running) {
    if (fd == ctx->sig_fd) {
        struct signalfd_siginfo fdsi;
        ssize_t fdsi_read_size = read(ctx->sig_fd, &fdsi, sizeof(fdsi));

        if (fdsi_read_size < 0) {
            syslog(LOG_ERR, "signalfd read failed: %m");
        }

        if (fdsi_read_size != sizeof(fdsi)) {
            syslog(LOG_ERR, "signalfd read: expected %zu bytes, got %zd", sizeof(fdsi), fdsi_read_size);
            return;
        }
        syslog(LOG_INFO, "Signal %u received, exiting event loop.", fdsi.ssi_signo);
        *running = 0;
    } else if (fd == ctx->timer_fd) {
        uint64_t expirations;
        read(ctx->timer_fd, &expirations, sizeof(expirations));
        fsm_update(EVENT_TIMEOUT);
    } else if (fd ==ctx->door_sensor_fd) {
        struct input_event ev;
        while (read(ctx->door_sensor_fd, &ev, sizeof(ev)) > 0) {
            door_sensor_process_event(&ev);
            fsm_update(door_sensor_is_open() ? EVENT_DOOR_OPEN : EVENT_DOOR_CLOSED);
        }
    } else if (fd == ctx->keypad_fd) {
        struct input_event ev;
        while (read(ctx->keypad_fd, &ev, sizeof(ev)) > 0) {
            KeyEvent k_ev = process_keypad_event(&ev);
            if (k_ev.cmd == KEY_CMD_NONE) continue;

            PinStatus ps = pin_entry_process_key(k_ev);
            if (ps == PIN_STATUS_VALID) {
                fsm_update(EVENT_PIN_VALID);
            } else if (ps == PIN_STATUS_INVALID) {
                fsm_update(EVENT_PIN_INVALID);
            }
            else if (ps == PIN_STATUS_CANCELLED) {
                fsm_update(EVENT_PIN_CANCEL);
            }
        }
    }
}

/**
 * @brief Application entry point and event orchestration loop.
 * * Initializes hardware drivers (PWM, GPIO), sets up the FSM,
 * and enters a non-blocking epoll_wait loop to process:
 * - Timer expirations (Auto-lock)
 * - GPIO edge triggers (Door sensor)
 * - Keypad input events (PIN entry)
 * - Linux signals (Graceful shutdown)
 * * @return int EXIT_SUCCESS on graceful shutdown, EXIT_FAILURE on init error.
 */
int main(int argc, const char* argv[]) {
    DaemonContext ctx = {};
    static int running = 1;
    static int sys_failure = 0;
    struct epoll_event events[MAX_EVENTS];
    int num_fds;

    openlog("door_security_daemon", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Door Security Daemon starting...");

    if (initialize_system(&ctx) < 0 || setup_event_polling(&ctx) < 0) {
        syslog(LOG_ERR, "Failed to initialize door security system.");
        sys_failure = 1;
        goto cleanup;
    }

    syslog(LOG_INFO, "System online. State: %s", fsm_state_to_string(fsm_get_state()));

    while (running) {
        num_fds = epoll_wait(ctx.epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds < 0 && errno != EINTR) {
            break;
        }

        for (int i =0; i < num_fds; i++) {
            dispatch_event(events[i].data.fd, &ctx, &running);
        }
    }

cleanup:
    syslog(LOG_INFO, "Shutting down...");
    lock_cleanup();
    close(ctx.keypad_fd);
    close(ctx.door_sensor_fd);
    close(ctx.timer_fd);
    close(ctx.sig_fd);
    close(ctx.epoll_fd);
    closelog();
    if (sys_failure > 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
