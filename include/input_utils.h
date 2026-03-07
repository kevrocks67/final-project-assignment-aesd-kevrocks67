/**
 * @file input_utils.h
 * @brief Utilities for discovering and identifying Linux input devices.
 * * This module parses a path, /proc/bus/input/devices by default, to
 * dynamically map device names (like "Keypad" or "GPIO") to their
 * corresponding /dev/input/eventX nodes.
 */

#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#define LINE_BUFFER_LEN 256

/** * @brief Path of the /proc devices file. This maps all
 */
static const char* devices_path = "/proc/bus/input/devices";

/**
 * @brief Scans devices_path for a specific device name.
 * * This function performs a safe, stack-based scan of the kernel's input
 * device list. It identifies the "event" handler number associated with
 * a device whose "Name=" field matches the provided string.
 * * @param device_name The substring to look for in the device Name field.
 * @return The event integer (e.g., 2 for /dev/input/event2) if found,
 * otherwise returns -1.
 * * @note This function is thread-safe as it uses local stack buffers
 * and standard file I/O.
 */
int find_input_event_num(const char* device_name);

/**
 * @brief Sets a custom path for the input devices file.
 * * @warning This is a **Test-Only API**. It is intended to be used by
 * unit testing frameworks (like Unity) to point the parser at a mock
 * text file instead of the live /proc filesystem.
 * * @param path The filesystem path to the mock devices file.
 */
void input_utils_set_path(const char* path);

#endif //INPUT_UTILS_H
