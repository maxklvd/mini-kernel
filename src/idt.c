/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "idt.h"

#include <intrin.h>

#include "gdt.h"
#include "isr.h"

__declspec(align(8))
idt_entry_t idt[NUM_INTERRUPTS] = {0};

__declspec(align(8))
idtr_t idtr = {sizeof(idt_entry_t) * NUM_INTERRUPTS - 1, (uint64_t)&idt};

const char* interrupt_names[NUM_INTERRUPTS] = {
    "Divide-By-Zero-Error",                     /*  0 */
    "Debug",                                    /*  1 */
    "Non-Maskable-Interrupt",                   /*  2 */
    "Breakpoint",                               /*  3 */
    "Overflow",                                 /*  4 */
    "Bound-Range",                              /*  5 */
    "Invalid-Opcode",                           /*  6 */
    "Device-Not-Available",                     /*  7 */
    "Double-Fault",                             /*  8 */
    "Coprocessor-Segment-Overrun",              /*  9 */
    "Invalid-TSS",                              /* 10 */
    "Segment-Not-Present",                      /* 11 */
    "Stack",                                    /* 12 */
    "General-Protection",                       /* 13 */
    "Page-Fault",                               /* 14 */
    "", /* Reserved */                          /* 15 */
    "Floating-Point Exception-Pending",         /* 16 */
    "Alignment-Check",                          /* 17 */
    "Machine-Check",                            /* 18 */
    "SIMD Floating-Point",                      /* 19 */
    "Reserved",                                 /* 20 */
    "Control - Protection",                     /* 21 */
    "", /* Reserved */                          /* 22 */
    "", /* Reserved */                          /* 23 */
    "", /* Reserved */                          /* 24 */
    "", /* Reserved */                          /* 25 */
    "", /* Reserved */                          /* 26 */
    "", /* Reserved */                          /* 27 */
    "Hypervisor Injection Exception",           /* 28 */
    "VMM Communication Exception",              /* 29 */
    "SVM Security Exception",                   /* 30 */
    "", /* Reserved (Internal and External) */  /* 31 */
};

void set_idt_entry(idt_entry_t* desc, int64_t isr) {
    desc->offset_1 = isr & 0xffff;
    desc->offset_2 = (isr >> 16) & 0xffff;
    desc->offset_3 = (isr >> 32) & 0xffffffff;
    desc->selector = KERNEL_CODE;
    desc->attributes = 0x8e;                    /* [ P DPL=0 GateType=64-bit Interrupt Gate ] */
}

void setup_idt(void) {
    set_idt_entry(&idt[0], (int64_t)&isr0);
    set_idt_entry(&idt[1], (int64_t)&isr1);
    set_idt_entry(&idt[2], (int64_t)&isr2);
    set_idt_entry(&idt[3], (int64_t)&isr3);
    set_idt_entry(&idt[4], (int64_t)&isr4);
    set_idt_entry(&idt[5], (int64_t)&isr5);
    set_idt_entry(&idt[6], (int64_t)&isr6);
    set_idt_entry(&idt[7], (int64_t)&isr7);
    set_idt_entry(&idt[8], (int64_t)&isr8);
    set_idt_entry(&idt[9], (int64_t)&isr9);
    set_idt_entry(&idt[10], (int64_t)&isr10);
    set_idt_entry(&idt[11], (int64_t)&isr11);
    set_idt_entry(&idt[12], (int64_t)&isr12);
    set_idt_entry(&idt[13], (int64_t)&isr13);
    set_idt_entry(&idt[14], (int64_t)&page_fault_wrapper);
    set_idt_entry(&idt[15], (int64_t)&isr15);
    set_idt_entry(&idt[16], (int64_t)&isr16);
    set_idt_entry(&idt[17], (int64_t)&isr17);
    set_idt_entry(&idt[18], (int64_t)&isr18);
    set_idt_entry(&idt[19], (int64_t)&isr19);
    set_idt_entry(&idt[20], (int64_t)&isr20);
    set_idt_entry(&idt[21], (int64_t)&isr21);
    set_idt_entry(&idt[22], (int64_t)&isr22);
    set_idt_entry(&idt[23], (int64_t)&isr23);
    set_idt_entry(&idt[24], (int64_t)&isr24);
    set_idt_entry(&idt[25], (int64_t)&isr25);
    set_idt_entry(&idt[26], (int64_t)&isr26);
    set_idt_entry(&idt[27], (int64_t)&isr27);
    set_idt_entry(&idt[28], (int64_t)&isr28);
    set_idt_entry(&idt[29], (int64_t)&isr29);
    set_idt_entry(&idt[30], (int64_t)&isr30);
    set_idt_entry(&idt[31], (int64_t)&isr31);

    __lidt(&idtr);
}
