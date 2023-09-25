/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef VIDEO_H
#define VIDEO_H

#include <efi.h>

void set_video_mode(int width, int height, int pixel_format);

/* Displays a pixel at specified coordinates */
void set_pixel(int x, int y, int color);

/* Fills the frame buffer with the specified color */
void fill(int color);

/* Gets the address of the frame buffer */
EFI_PHYSICAL_ADDRESS frame_buffer(void);

#endif /* VIDEO_H */
