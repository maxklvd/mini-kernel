/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <efi.h>

/* Default background color */
#define COLOR_BACKGROUND    0x101010

/* Colors that are used for text output */
#define COLOR_TEXT          0x909090
#define COLOR_WHITE         0xffffff
#define COLOR_YELLOW        0xffff80
#define COLOR_RED           0xff8080
#define COLOR_GREEN         0x80CC80

/* Sets the text color */
void set_color(int color);

/* Clears the screen */
void clear(void);

/*
 * Prints the specified formatted string to the screen
 * Supports only:
 *  - string (%s),
 *  - hex-value (%x or %04x),
 *  - newline (\n),
 *  - tab (\t).
 */
void printk(const char* fmt, ...);

/* Same as printk, but the first argument is the text color */
void printc(int color, const char* fmt, ...);

#endif /* OUTPUT_H */
