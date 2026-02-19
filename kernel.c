#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "keyboard_driver.h"



static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;



static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | ((uint16_t)color << 8);
}

static size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

static int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

static int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

static void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*)0xB8000;

    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            terminal_putentryat(' ', terminal_color, x, y);
}

static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            terminal_buffer[(y - 1) * VGA_WIDTH + x] =
                terminal_buffer[y * VGA_WIDTH + x];

    for (size_t x = 0; x < VGA_WIDTH; x++)
        terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);

    terminal_row = VGA_HEIGHT - 1;
}

static void terminal_hidecursor(void) {
    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
}

static void terminal_showcursor(void) {
    terminal_putentryat('_', terminal_color, terminal_column, terminal_row);
}

static void terminal_putchar(char c) {
    terminal_hidecursor();

    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_scroll();
    } else {
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
                terminal_scroll();
        }
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
    }

    terminal_showcursor();
}

static void terminal_writestring(const char* data) {
    for (size_t i = 0; i < strlen(data); i++)
        terminal_putchar(data[i]);
}

static void terminal_backspace(void) {
    terminal_hidecursor();

    if (terminal_column > 0) {
        terminal_column--;
    } else if (terminal_row > 0) {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    }

    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    terminal_showcursor();
}



static void int_to_string(int value, char* str) {
    char buffer[32];
    int i = 0;
    bool negative = false;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    if (value < 0) {
        negative = true;
        value = -value;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (negative)
        buffer[i++] = '-';

    int j = 0;
    while (i > 0)
        str[j++] = buffer[--i];

    str[j] = '\0';
}

static int string_to_int(const char* str, size_t* index) {
    int result = 0;
    bool negative = false;

    if (str[*index] == '-') {
        negative = true;
        (*index)++;
    }

    while (str[*index] >= '0' && str[*index] <= '9') {
        result = result * 10 + (str[*index] - '0');
        (*index)++;
    }

    return negative ? -result : result;
}

static void calculate_expression(const char* input) {
    size_t i = 5; // после "!calc"

    while (input[i] == ' ') i++;

    int a = string_to_int(input, &i);

    while (input[i] == ' ') i++;

    char op = input[i++];

    while (input[i] == ' ') i++;

    int b = string_to_int(input, &i);

    int result;

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                terminal_writestring("Error: Division by zero\n");
                return;
            }
            result = a / b;
            break;
        default:
            terminal_writestring("Invalid operator. Use + - * /\n");
            return;
    }

    char buffer[32];
    int_to_string(result, buffer);

    terminal_writestring("Result: ");
    terminal_writestring(buffer);
    terminal_writestring("\n");
}



static void terminal_readstring(char* buffer, size_t max_length) {
    size_t length = 0;

    while (length < max_length - 1) {
        char c = keyboard_read();

        if (c == '\n') {
            terminal_putchar('\n');
            break;
        } else if (c == '\b') {
            if (length > 0) {
                length--;
                terminal_backspace();
            }
        } else if (c >= ' ') {
            buffer[length++] = c;
            terminal_putchar(c);
        }
    }

    buffer[length] = '\0';
}



static void handle_input(char* input) {

    if (strcmp(input, "!ZenOS") == 0) {
        terminal_writestring("                              -=====+=---::. :%@%##%@%+.            \n");
        terminal_writestring("                             +@@@@%*######%%%%#+++++**#%+           \n");
        terminal_writestring("                            :@@@@@%++++++++++++*%%%%#+++#%=         \n");
        terminal_writestring("                            *@@@@@%++++++++++#%@@**#@%*++*%#.       \n");
        terminal_writestring("           =*=            :=@@@@@@%+++++++++@@#*++++*%@#+++#%=      \n");
        terminal_writestring("        .*@@@@@+-     :+#@@@@@@@@@%+++++++++%@*+++++++*%%*++*@%:    \n");
        terminal_writestring("        #@@@@@@@@@#+#@@@@@@@@@@@@@%+++++++*##@@+++++++++*%#+++#@:   \n");
        terminal_writestring("         %@@@@@@@@@@@@@@@@@@@@@@@@%====++*@@#%@#+++++++++++++++*@.  \n");
        terminal_writestring("         .@@@@@@@@@@@@@@@@@@@@@@@@%======*@@+*@@++++++++++++++++%%  \n");
        terminal_writestring("          =@@@@@@@@@@@@@@@@@@@@@@@%======%@@*+%@*+++++++++++++++*@- \n");
        terminal_writestring("          -@@@@@@@@@@@@@@@@@@@@@@@%====*%@%@%=#@%++++++++++++++++%# \n");
        terminal_writestring("          #@@@@@@@@@@@@@@@@@@@@@@@%*#%%*#@#%@++@@+=++++++++++++++#@ \n");
        terminal_writestring("         +@@@@@@@@@@@@@@@@@@@@@@@@@@#:.=@%#%@+=#@+==+++++++++++++*@.\n");
        terminal_writestring("      .-#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%@%==+@#===++++++++++++#% \n");
        terminal_writestring(" :+#%@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@++*#####*====@%====++++++++++*@# \n");
        terminal_writestring("-@@@@@@@@@@@@@@@@@@@@@@@@@@@@#: -@@%=============%%=====++++++++*@* \n");
        terminal_writestring("+@@@@@@@@@@@@@@@@@@@@@@@@@@@#   -@@%=============%@=====++++++++*@: \n");
        terminal_writestring("*@@@@@@@@@@@@@@@@@@@@@@@@@@@=   -@@%======*%+====%@+====+++++++++#@:\n");
        terminal_writestring("*@@@@@@@@@@@@@@@@@@@@@@@@@@@*   -@@%======+@@====#@=====++++++++++@%\n");
        terminal_writestring("=@@@@@@@@@@@@@@@@@@@@@@@@@@@@+  -@@%=======%@*===%@=====++++++++++@*\n");
        terminal_writestring(" :-+#%@@@@@@@@@@@@@@@@@@@@@@@@@##@@%=======*@%==+@%====++++++++++%@-\n");
        terminal_writestring("      .-%@@@@@@@@@@@@@@@@@@@@@@@@@%=======*@%==*@*====++++++++*@%:  \n");
        terminal_writestring("         *@@@@@@@@@@@@@@@@@@@@@@@@%=======*@%=+@%+===++++++++@@-    \n");
        terminal_writestring("          #@@@@@@@@@@@@@@@@@@@@@@@%=======*@@#%@*===+++++++++@@.    \n");
        terminal_writestring("          -@@@@@@@@@@@@@@@@@@@@@@@%=======*@@@@#==+++++++++++#@+    \n");
        terminal_writestring("          :@@@@@@@@@@@@@@@@@@@@@@@%=======*@@@@*=+++++++++++++@@    \n");
        terminal_writestring("          %@@@@@@@@@@@@@@@@@@@@@@@%=======#@#*@@*+++++++++++++#@-   \n");
        terminal_writestring("         #@@@@@@@@@@@@@@@@@@@@@@@@%=======#@# *@@+++++++++**++#@-   \n");
        terminal_writestring("        *@@@@@@@@@@#%@@@@@@@@@@@@@%+++++++*@#  #@@%%@@%%#*#%*#@*    \n");
        terminal_writestring("        :#@@@@@@*-   :=*@@@@@@@@@@%++++++++@@#%%##**+***#@@@@%=     \n");
        terminal_writestring("          :*%+:          :-*@@@@@@%++++++++%@*+++++++++++*@+        \n");
        terminal_writestring("                            #@@@@@%++++++++*@@*+***+++++++@*        \n");
        terminal_writestring("                            -@@@@@%+++++++++*@@@@@@++++++#@=:.      \n");
        terminal_writestring("                             *@@@@@*++++++++++##%%*+++++#@@@%=      \n");
        terminal_writestring("                              =+++#%@%##*************##%@%*-        \n");

    } else if (strncmp(input, "!calc", 5) == 0) {

        calculate_expression(input);

    } else if (strcmp(input, "!clear") == 0) {

        terminal_initialize();

    } else if (strcmp(input, "!help") == 0) {

        terminal_writestring("Available commands:\n");
        terminal_writestring("!ZenOS  - Display ZenOS logo\n");
        terminal_writestring("!calc   - Simple calculator\n");
        terminal_writestring("!clear  - Clear screen\n");
        terminal_writestring("!help   - Show commands\n");

    } else {

        terminal_writestring("Unknown command. Type !help\n");
    }
}



void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("Welcome To ZenOS\n");

    while (true) {
        terminal_writestring("ZenOS> ");

        char input[256];
        terminal_readstring(input, sizeof(input));

        if (strlen(input) > 0)
            handle_input(input);
    }
}
