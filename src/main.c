/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include <efi.h>
#include <efilib.h>

#include "demo.h"
#include "paging.h"
#include "util.h"
#include "video.h"

/* Increase the size if it's not big enough for the GetMemoryMap call */
#define MEMORY_MAP_SIZE 0x2000

typedef struct {
    UINT8  memory_map[MEMORY_MAP_SIZE];
    UINTN  size;
    UINTN  key;
    UINTN  descriptor_size;
    UINT32 descriptor_version;
} MemoryMap;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE*SystemTable) {
    MemoryMap map = {0};

    InitializeLib(ImageHandle, SystemTable);

    /* The pause() function will have a delay of 0.5 seconds */
    init_pause(SystemTable, 500000);

    /*
     * Try to set the mode to 1024x768.
     * A pixel is 32-bits and byte zero represents blue, byte one represents green, byte two represents red, and byte three is reserved.
     */
    set_video_mode(1024, 768, PixelBlueGreenRedReserved8BitPerColor);

    EFI_PHYSICAL_ADDRESS addr = frame_buffer();
    if (addr > MAX_PHYSICAL_ADDRESS) {
        Print(L"Error: frame buffer address (0x%lx) is not mapped\n", addr);
        halt();
    }

    /* Exit boot services */

    /* For simplicity, the buffer size is specified as fixed and should have enough space */
    map.size = MEMORY_MAP_SIZE;

    EFI_STATUS status = BS->GetMemoryMap(&map.size,
                                         (EFI_MEMORY_DESCRIPTOR*)map.memory_map,
                                         &map.key,
                                         &map.descriptor_size,
                                         &map.descriptor_version);
    check_status(status, EFI_SUCCESS, L"GetMemoryMap");
    BS->ExitBootServices(ImageHandle, map.key);

    /* Now let's run the demo and stop the program */
    start_demo();
    halt();

    return EFI_SUCCESS;
}
