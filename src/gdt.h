/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef GDT_H
#define GDT_H

#include <efi.h>

#define KERNEL_CODE     0x08
#define KERNEL_DATA     0x10
#define USER_NULL       0x18
#define USER_DATA       0x20
#define USER_CODE       0x28
#define TSS_LOW         0x30
#define TSS_HIGH        0x38

#pragma pack(push, 1)

typedef struct {
    uint64_t null;                              /* 0x00 */
    uint64_t code;                              /* 0x08 */
    uint64_t data;                              /* 0x10 */
    uint64_t null_user;                         /* 0x18 */
    uint64_t user_data;                         /* 0x20 */
    uint64_t user_code;                         /* 0x28 */
    uint64_t tss_low;                           /* 0x30 */
    uint64_t tss_high;                          /* 0x38 */
} gdt_t;

typedef struct {
    uint16_t limit;
    uint64_t address;
} gdtr_t;

typedef struct {
    uint32_t reserve1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserve2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserve3;
    uint16_t reserve4;
    uint16_t iomap_base;
} tss_t;

#pragma pack(pop)

void setup_gdt(void);

#endif /* GDT_H */
