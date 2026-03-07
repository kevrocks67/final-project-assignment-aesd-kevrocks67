#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input_utils.h"

int find_input_event_num(const char* device_name) {
    int event_num = -1;
    int found_device = 0;

    FILE* input_devices_fp = fopen(devices_path, "r");
    if (input_devices_fp == NULL) {
        return -2;
    }

    const char* event_tok = "event";
    size_t event_tok_len = strlen(event_tok);
    char line_buffer[LINE_BUFFER_LEN];

    while(fgets(line_buffer, LINE_BUFFER_LEN, input_devices_fp)) {
        // Look for the device we want
        if(strstr(line_buffer, "Name=") && strstr(line_buffer, device_name)) {
            found_device = 1;
        }

        // When we find the right device, parse for event ID
        if (found_device && strstr(line_buffer, "Handlers=")) {
            // Get a pointer to the word event in `Handlers=eventX`
            char* event_tok_ptr = strstr(line_buffer, event_tok);

            if (event_tok_ptr) {
                size_t event_tok_ptr_len = strlen(event_tok_ptr);

                // Ensure there is something defined after the word event
                if (event_tok_ptr_len > event_tok_len) {
                    // Advance pointer past the word event
                    char* event_num_start = event_tok_ptr + event_tok_len;
                    char* endptr;

                    // Convert string to long with explicit error checking
                    long val = strtol(event_num_start, &endptr, 10);

                    // If endptr moved, we successfully parsed at least one digit
                    if (event_num_start != endptr) {
                        event_num = (int)val;
                        break;
                    }
                }

            }
        }

        // Blank line indicating another device is being defined
        if (line_buffer[0] == '\n' || line_buffer[0] == '\r') {
            found_device = 0;
        }
    }

    fclose(input_devices_fp);
    return event_num;
}

void input_utils_set_path(const char* path) {
    devices_path = path;
}
