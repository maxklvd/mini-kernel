/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <efi.h>

/* Configures syscall and sysret calls */
void setup_syscall(unsigned syscall_cs_ss, unsigned sysret_cs_ss);

/* Calls the specified function in user mode through sysret */
void usermode_enter(void* user_function);

/* Returns to the kernel from user mode through syscall */
void usermode_exit(void);

#endif /* SYSCALL_H */
