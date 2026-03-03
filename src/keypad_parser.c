#include "keypad_parser.h"
#include <linux/input.h>
#include <string.h>

KeyEvent process_keypad_event(const struct input_event* ev) {
    KeyEvent result;

    result.cmd = KEY_CMD_NONE;
    memset(&result.data, 0, sizeof(result.data));

    if (ev->type == EV_KEY && ev->value == 1) {
        switch (ev->code) {
            case KEY_0:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 0;
                break;
            case KEY_1:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 1;
                break;
            case KEY_2:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 2;
                break;
            case KEY_3:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 3;
                break;
            case KEY_4:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 4;
                break;
            case KEY_5:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 5;
                break;
            case KEY_6:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 6;
                break;
            case KEY_7:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 7;
                break;
            case KEY_8:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 8;
                break;
            case KEY_9:
                result.cmd = KEY_CMD_NUMBER;
                result.data.key_number = 9;
                break;
            case KEY_ENTER:
                result.cmd = KEY_CMD_SUBMIT;
                break;
            case KEY_KPASTERISK:
                result.cmd = KEY_CMD_CANCEL;
                break;
            default:
                break;
        }
    }
    return result;
}
