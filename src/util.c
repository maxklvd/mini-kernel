/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "util.h"

#include <efilib.h>
#include <intrin.h>

#include "output.h"

uint64_t pause_ticks = 0;

/* Initializes the pause function */
void init_pause(EFI_SYSTEM_TABLE* SystemTable, uint64_t delay) {
    uint64_t ticks;

    ticks = __rdtsc();
    SystemTable->BootServices->Stall(delay);    /* in microseconds */
    pause_ticks = __rdtsc() - ticks;
}

/* Suspends program execution for the time specified in the init_pause function */
void pause(void) {
    uint64_t end;

    end = __rdtsc() + pause_ticks;
    while (__rdtsc() < end) { /* waiting */ }
}

/* Checks the status of EFI function execution. Terminates the program if an error occurs */
void check_status(EFI_STATUS status, EFI_STATUS expected, CHAR16*msg) {
    if (status != expected) {
        Print(L"ERROR: %s (EFI STATUS %r)\n", msg, status);
        halt();
    }
}

/* Stops program execution */
void halt(void) {
    for (;;) {
        __halt();
    }
}
