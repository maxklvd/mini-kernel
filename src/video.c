/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "video.h"

#include <efi.h>
#include <efilib.h>

#include "util.h"

EFI_PHYSICAL_ADDRESS frame_buffer_base = 0;
int screen_width    = 0;
int screen_height   = 0;

void set_video_mode(int width, int height, int pixel_format) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_HANDLE* handle_buffer;
    UINTN handle_count = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode_info;
    UINTN size_of_info;
    EFI_STATUS status;

    status = gBS->LocateHandleBuffer(ByProtocol,
                                     &gEfiGraphicsOutputProtocolGuid,
                                     NULL,
                                     &handle_count,
                                     &handle_buffer);
    check_status(status, EFI_SUCCESS, L"LocateHandleBuffer() failed");

    status = gBS->HandleProtocol(handle_buffer[0],
                                 &gEfiGraphicsOutputProtocolGuid,
                                 (VOID**)&gop);
    check_status(status, EFI_SUCCESS, L"HandleProtocol() failed");

    UINT32 mode_num = 0;
    for (; (status = gop->QueryMode(gop, mode_num, &size_of_info, &gop_mode_info)) == EFI_SUCCESS;
           mode_num++) {
        if (gop_mode_info->HorizontalResolution == width &&
            gop_mode_info->VerticalResolution == height &&
            gop_mode_info->PixelFormat == pixel_format)
            break;
    }
    check_status(status, EFI_SUCCESS, L"QueryMode() failed");

    status = gop->SetMode(gop, mode_num);
    check_status(status, EFI_SUCCESS, L"SetMode() failed");

    frame_buffer_base = gop->Mode->FrameBufferBase;
    screen_width = width;
    screen_height = height;
}

/* Displays a pixel at specified coordinates */
void set_pixel(int x, int y, int color) {
    uint32_t* ptr = ((uint32_t*)frame_buffer_base) + ((y * screen_width) + x);
    *ptr = color;
}

/* Fills the frame buffer with the specified color */
void fill(int color) {
    uint32_t* ptr = (uint32_t*)frame_buffer_base;
    int num_pixels = screen_width * screen_height;

    while (num_pixels--) {
        *ptr++ = color;
    }
}

/* Gets the address of the frame buffer */
EFI_PHYSICAL_ADDRESS frame_buffer(void) {
    return frame_buffer_base;
}
