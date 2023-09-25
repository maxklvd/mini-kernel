/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef IDT_H
#define IDT_H

#include <efi.h>

#define NUM_INTERRUPTS  32

#pragma pack(push, 1)

typedef struct {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t reserved;
} idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t address;
} idtr_t;

#pragma pack(pop)

extern const char* interrupt_names[];

void setup_idt(void);

#endif /* IDT_H */
