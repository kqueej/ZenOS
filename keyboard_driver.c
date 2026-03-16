#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64


static const char keyboard_map[256] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


static const char shift_keyboard_map[256] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


#define SHIFT_LEFT 0x2A
#define SHIFT_RIGHT 0x36
#define CTRL_LEFT 0x1D
#define ALT_LEFT 0x38
#define SHIFT_LEFT_RELEASE 0xAA
#define SHIFT_RIGHT_RELEASE 0xB6
#define CTRL_LEFT_RELEASE 0x9D
#define ALT_LEFT_RELEASE 0xB8

static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


char keyboard_read(void) {
    uint8_t scan_code;
    char ch = 0;


    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0) {

    }

    scan_code = inb(KEYBOARD_DATA_PORT);


    switch (scan_code) {
        case SHIFT_LEFT:
        case SHIFT_RIGHT:
            shift_pressed = true;
            return 0;
        case SHIFT_LEFT_RELEASE:
        case SHIFT_RIGHT_RELEASE:
            shift_pressed = false;
            return 0;
        case CTRL_LEFT:
            ctrl_pressed = true;
            return 0;
        case CTRL_LEFT_RELEASE:
            ctrl_pressed = false;
            return 0;
        case ALT_LEFT:
            alt_pressed = true;
            return 0;
        case ALT_LEFT_RELEASE:
            alt_pressed = false;
            return 0;
        default:
            break;
    }


    if (shift_pressed) {
        ch = shift_keyboard_map[scan_code];
    } else {
        ch = keyboard_map[scan_code];
    }

    return ch;
}


void keyboard_initialize(void) {

    outb(KEYBOARD_STATUS_PORT, 0xFF);
}
