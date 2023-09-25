/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef UTIL_H
#define UTIL_H

#include <efi.h>

/* Initializes the pause function */
void init_pause(EFI_SYSTEM_TABLE* SystemTable, uint64_t delay);

/* Suspends program execution for the time specified in the init_pause function */
void pause(void);

/* Checks the status of EFI function execution. Terminates the program if an error occurs */
void check_status(EFI_STATUS status, EFI_STATUS expected, CHAR16* msg);

/* Gets the CS register value */
unsigned read_cs(void);

/* Gets the SS register value */
unsigned read_ss(void);

/* Gets the task register value */
unsigned read_tr(void);

/* Sets the task register value */
void write_tr(unsigned tss_sel);

/* Reloads segment registers CS, DS, ES, FS, GS, and SS */
void reload_registers(unsigned code_sel, unsigned data_sel);

/* Stops program execution */
void halt(void);

#endif /* UTIL_H */
