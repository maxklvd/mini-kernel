/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "output.h"

#include <stdarg.h>

#include "font.h"
#include "video.h"

#define TEXT_START_X    10
#define TEXT_START_Y    10

#define LINE_HEIGHT     22
#define LETTER_SPACING  1
#define TAB_WIDTH       20

int text_x = TEXT_START_X;
int text_y = TEXT_START_Y;
int text_color = COLOR_TEXT;

extern int screen_width;
extern int screen_height;

void clear(void) {
    fill(COLOR_BACKGROUND);
    text_x = TEXT_START_X;
    text_y = TEXT_START_Y;
}

void newline(void) {
    text_x = TEXT_START_X;
    text_y += LINE_HEIGHT;
}

void tabulate(void) {
    text_x = (text_x / TAB_WIDTH + 1) * TAB_WIDTH;
}

/* Sets the current text color */
void set_color(int color) {
    text_color = color;
}

/*
 * Displays a symbol on the screen using a bitmap font.
 * The character will be output at position (text_x, text_y).
 */
void put_char(char c) {
    int data_idx;
    font_char_t ch;

    if (c < FONT_CHAR_FROM || c > FONT_CHAR_TO) {
        c = '?';                                /* Unknown character */
    }

    ch = char_map[c - FONT_CHAR_FROM];
    if (text_x + ch.width > screen_width) {
        newline();
    }

    data_idx = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < ch.width; x++) {
            uint8_t data = ch.bits[data_idx++];

            for (int i = 0; i < 8; i++) {
                if ((data >> i) & 1) {
                    set_pixel(text_x + x, text_y + y * 8 + i, text_color);
                }
            }
        }
    }

    text_x += ch.width + LETTER_SPACING;
}

/* Prints the specified string to the screen */
void print_text(const char* s) {
    while (*s) {
        put_char(*s++);
    }
}

/* Number of non-zero digits */
int num_digits(uint64_t v) {
    int digits = 1;
    int cnt = 1;

    while (cnt <= 16) {
        if (v & 0x0F) {
            digits = cnt;
        }
        v >>= 4;
        cnt++;
    }

    return digits;
}

/* Left padding with zeros, if any */
int leading_zeros(const char** fmt) {
    switch (**fmt) {
        case 'x':
            return 0;
        case '0': {
            (*fmt)++;

            int zeros = 0;

            while (**fmt != 'x') {
                char c = **fmt;
                if (c >= '0' && c <= '9') {
                    zeros *= 10;
                    zeros += (c - '0');
                } else {
                    return -1;
                }
                (*fmt)++;
            }
            return zeros > 16 ? 16 : zeros;
        }
        default:
            return -1;
    }
}

/* Prints a hexadecimal number */
void print_hex(uint64_t v, const char** fmt) {
    int padding = leading_zeros(fmt);
    if (padding >= 0) {
        int digits = num_digits(v);
        int num = digits > padding ? digits : padding;

        while (num--) {
            int shift = num << 2;
            uint64_t digit = (v & ((uint64_t)0x0f << shift)) >> shift;
            put_char("0123456789abcdef"[digit]);
        }
    }
}

void print_base(const char* fmt, va_list args) {
    if (text_y > screen_height - LINE_HEIGHT) {
        clear();
    }

    while (*fmt) {
        switch (*fmt) {
            case '%': {
                fmt++;

                switch (*fmt) {
                    case 's':
                        print_text(va_arg(args, char*));
                        break;
                    case '0':
                    case 'x':
                        print_hex(va_arg(args, uint64_t), &fmt);
                        break;
                    case '%':
                        put_char('%');
                        break;
                    default:
                        break;
                }
                break;
            }
            case '\n':
                newline();
                break;
            case '\t':
                tabulate();
                break;
            default:
                put_char(*fmt);
                break;
        }

        fmt++;
    }
}

/*
 * Prints the specified formatted string to the screen
 * Supports only:
 *  - string (%s),
 *  - hex-value (%x or %04x),
 *  - newline (\n),
 *  - tab (\t).
 */
void printk(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print_base(fmt, args);
    va_end(args);
}

/* Same as printk, but the first argument is the text color */
void printc(int color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int prev = text_color;
    set_color(color);
    print_base(fmt, args);
    set_color(prev);

    va_end(args);
}
