/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "isr.h"

#include <intrin.h>

#include "idt.h"
#include "output.h"
#include "paging.h"
#include "syscall.h"
#include "util.h"

#define NO_ERROR_CODE   (-1)

#define PF_BIT_P        (1 << 0)
#define PF_BIT_RW       (1 << 1)
#define PF_BIT_US       (1 << 2)
#define PF_BIT_RSV      (1 << 3)
#define PF_BIT_ID       (1 << 4)
#define PF_BIT_PK       (1 << 5)
#define PF_BIT_SS       (1 << 6)
#define PF_BIT_RMP      (1 << 31)

extern uint64_t pg_pd[];

/* Common handler for all interrupts 0..31, except #PF */
void isr_common(int irq_num, int64_t error_code) {
    /* Display the interrupt number and error code, if any */
    printc(COLOR_RED, "[INTERRUPT 0x%02x %s]", irq_num, interrupt_names[irq_num]);
    if (error_code != NO_ERROR_CODE) {
        printc(COLOR_RED, " Error Code: 0x%08x", error_code);
    }
    printk("\n");

    /* Interrupt a user task if the interruption occurred in user mode */
    usermode_exit();

    /* Stop the program if the interruption occurred in kernel mode */
    printc(COLOR_RED, " STOP");
    halt();
}

/* Displays a detailed error code for #PF */
void print_page_fault_error(uint32_t code) {
    const char* p   = code & PF_BIT_P   ? "P "   : "";
    const char* rw  = code & PF_BIT_RW  ? "R/W " : "";
    const char* us  = code & PF_BIT_US  ? "U/S " : "";
    const char* rsv = code & PF_BIT_RSV ? "RSV " : "";
    const char* id  = code & PF_BIT_ID  ? "I/D " : "";
    const char* pk  = code & PF_BIT_PK  ? "PK "  : "";
    const char* ss  = code & PF_BIT_SS  ? "SS "  : "";
    const char* rmp = code & PF_BIT_RMP ? "RMP " : "";

    printc(COLOR_RED, "Error Code: 0x%08x [ %s%s%s%s%s%s%s%s] ", code, p, rw, us, rsv, id, pk, ss, rmp);
}

/* Page-Fault exception handler */
void isr_page_fault(uint32_t error) {
    uint64_t vaddr;                             /* Virtual address */

    /* Display the exception number */
    printc(COLOR_RED, "[INTERRUPT 0x%02x %s] ", 14, interrupt_names[14]);
    /* Detailed error code */
    print_page_fault_error(error);

    /* Get the virtual address due to which #PF occurred */
    vaddr = __readcr2();
    printc(COLOR_RED, "Virtual Address: 0x%016x ", vaddr);

    /* Process only a certain range of addresses */
    if (vaddr < PAGE_SIZE_2M) {                         /* Low 2 MB */
        if (error & PF_BIT_P && error & PF_BIT_RW) {    /* The process tried to write a page */
            /* Change the page to writable */
            pg_pd[0] |= PAGE_WRITE;
            __invlpg((void*)vaddr);

            /* Display a message and return control to user mode */
            printc(COLOR_RED, "Set R/W Bit. Continue execution\n");
            return;
        }

        if (error & PF_BIT_P && error & PF_BIT_US) {    /* Access to a supervised page */
            /* Display a message and abort the task in user mode */
            printc(COLOR_RED, "Exit from user mode\n");
            usermode_exit();
            /* Stop */
        }
    }

    /* Stop the program */
    printc(COLOR_RED, " STOP");
    halt();
}

/* Interrupt handlers */
void isr0(void) { isr_common(0, NO_ERROR_CODE); }
void isr1(void) { isr_common(1, NO_ERROR_CODE); }
void isr2(void) { isr_common(2, NO_ERROR_CODE); }
void isr3(void) { isr_common(3, NO_ERROR_CODE); }
void isr4(void) { isr_common(4, NO_ERROR_CODE); }
void isr5(void) { isr_common(5, NO_ERROR_CODE); }
void isr6(void) { isr_common(6, NO_ERROR_CODE); }
void isr7(void) { isr_common(7, NO_ERROR_CODE); }
void isr8(void) { isr_common(8, irq_error_code()); }
void isr9(void) { isr_common(9, NO_ERROR_CODE); }
void isr10(void) { isr_common(10, irq_error_code()); }
void isr11(void) { isr_common(11, irq_error_code()); }
void isr12(void) { isr_common(12, irq_error_code()); }
void isr13(void) { isr_common(13, irq_error_code()); }
/* 14: isr_page_fault */
void isr15(void) { isr_common(15, NO_ERROR_CODE); }
void isr16(void) { isr_common(16, NO_ERROR_CODE); }
void isr17(void) { isr_common(17, irq_error_code()); }
void isr18(void) { isr_common(18, NO_ERROR_CODE); }
void isr19(void) { isr_common(19, NO_ERROR_CODE); }
void isr20(void) { isr_common(20, NO_ERROR_CODE); }
void isr21(void) { isr_common(21, NO_ERROR_CODE); }
void isr22(void) { isr_common(22, NO_ERROR_CODE); }
void isr23(void) { isr_common(23, NO_ERROR_CODE); }
void isr24(void) { isr_common(24, NO_ERROR_CODE); }
void isr25(void) { isr_common(25, NO_ERROR_CODE); }
void isr26(void) { isr_common(26, NO_ERROR_CODE); }
void isr27(void) { isr_common(27, NO_ERROR_CODE); }
void isr28(void) { isr_common(28, NO_ERROR_CODE); }
void isr29(void) { isr_common(29, NO_ERROR_CODE); }
void isr30(void) { isr_common(30, NO_ERROR_CODE); }
void isr31(void) { isr_common(31, NO_ERROR_CODE); }
