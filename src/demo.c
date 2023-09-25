/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "demo.h"

#include <intrin.h>

#include "gdt.h"
#include "idt.h"
#include "output.h"
#include "paging.h"
#include "syscall.h"
#include "util.h"

#define NUM_USER_TASKS  4

void run_user_task_1(void);
void run_user_task_2(void);
void run_user_task_3(void);
void run_user_task_4(void);

/* Displays the current privilege level */
void print_current_ring(void) {
    printc(COLOR_YELLOW, "[ring %x] ", (read_cs() & 0b11));
}

/* Displays the task title */
void print_title(int num, const char* title) {
    printc(COLOR_WHITE, "(%x / %x) ", num, NUM_USER_TASKS);
    printk("%s\n", title);
}

void start_demo(void) {
    gdtr_t gdt_ptr = {0};
    idtr_t idt_ptr = {0};

    const char* separator =
        "---------------------------------------------------------"
        "---------------------------------------------------------"
        "---------------------------------------------------------"
        "---------------------------------------------------------";

    /* Clear the screen */
    clear();
    set_color(COLOR_TEXT);

    /* Display a welcome message */
    printc(COLOR_GREEN, "Welcome to Mini-Kernel Demo! (x86_64)\n");
    printc(COLOR_GREEN, separator);
    printk("\n\n");
    pause();

    /* Set the global descriptor table */
    printk("Setup new GDT:\t\t\t\t");
    setup_gdt();

    printc(COLOR_GREEN, "OK");
    _sgdt(&gdt_ptr);
    printk(" gdtr = 0x%x, limit = 0x%x\n", gdt_ptr.address, gdt_ptr.limit);
    printk("\t\t\t\t\t\t\t\t\t");
    printk("0x%02x - kernel code, 0x%02x - kernel data, 0x%02x - user null, 0x%02x - user data, "
           "0x%02x - user code, 0x%02x - tss low, 0x%02x - tss high\n",
           KERNEL_CODE, KERNEL_DATA, USER_NULL, USER_DATA, USER_CODE, TSS_LOW, TSS_HIGH);

    /* Assign new values for the cs, ds, es, fs, gd, ss, and tr registers */
    printk("Reload registers:\t\t\t\t");

    write_tr(TSS_LOW);
    reload_registers(KERNEL_CODE, KERNEL_DATA);

    printc(COLOR_GREEN, "OK");
    printk(" cs = 0x%02x [CPL=%x], ss = 0x%02x [RPL=%x], tr = 0x%02x\n",
           read_cs(),
           (read_cs() & 0b11),
           read_ss(),
           (read_ss() & 0b11),
           read_tr());

    /* Set the interrupt descriptor table */
    printk("Setup new IDT:\t\t\t\t");
    setup_idt();

    printc(COLOR_GREEN, "OK");
    __sidt(&idt_ptr);
    printk(" idtr = 0x%x, limit = 0x%x, 0...31 interrupts\n", idt_ptr.address, idt_ptr.limit);

    printk("Setup identity paging:\t\t\t");
    /* 1-1 mapping of physical memory */
    setup_identity_paging();

    /*
     * See the result of the 'info mem' command from Qemu Monitor at this point.
     * There should be 0000000000000000 - 0000008000000000 0000008000000000 urw
     */

    printc(COLOR_GREEN, "OK");
    printk(" cr3 = 0x%x, Physical-Page Size: 2 Mbyte, 0x%016x...0x%016x\n",
           __readcr3(),
           MIN_PHYSICAL_ADDRESS,
           MAX_PHYSICAL_ADDRESS);

    /* Configure calls from the kernel to user mode and back from user mode to the kernel */
    printk("Enable SYSCALL/SYSRET:\t\t");

    /*
     * SYSCALL selectors will be:
     *     CS = KERNEL_CODE (0x08)
     *     SS = KERNEL_CODE + 0x08 (0x10)
     * SYSRET selectors will be:
     *     CS = USER_NULL + 0x18 (0x2b [CPL=3])
     *     SS = USER_NULL + 0x08 (0x23 [RPL=3])
     */
    setup_syscall(KERNEL_CODE, USER_NULL);

    printc(COLOR_GREEN, "OK\n\n");

    printc(COLOR_GREEN, separator);
    printk("\n\n");
    pause();

    /* Kernel setup is complete here */

    /* Display the current privilege level */
    print_current_ring();
    printk("Ready to run user tasks\n\n");
    pause();

    /* Now let's run tasks in user mode */
    run_user_task_1();
    run_user_task_2();
    run_user_task_3();
    run_user_task_4();

    /* Demonstration completed */
    printc(COLOR_GREEN, "%s\nDone", separator);
}

/* Task 1. Enter user mode and exit it */
void user_task_1(void) {
    print_current_ring();
    printk("Done. Registers: cs = 0x%02x [CPL=%x], ss = 0x%02x [RPL=%x]\n",
           read_cs(),
           (read_cs() & 0b11),
           read_ss(),
           (read_ss() & 0b11));
    /* Back to the kernel */
}

void run_user_task_1(void) {
    print_title(1, "Enter to user mode");
    pause();

    /* Go to user mode */
    usermode_enter(&user_task_1);
    pause();
    printk("\n");
}

/* Task 2. Executes the privileged instruction. The task will be terminated by exception #GP */
void user_task_2(void) {
    print_current_ring();
    printk("Executing HLT instruction\n");

    __halt();                                   /* #GP is here */
}

void run_user_task_2(void) {
    print_title(2, "Try to execute a privileged instruction in user mode (#GP expected)");
    pause();

    /* Go to user mode */
    usermode_enter(&user_task_2);
    pause();
    printk("\n");
}

/* Task 3. Reads data from the supervisor page. The task will be terminated by exception #PF */
void user_task_3(void) {
    uint8_t* pointer;
    uint8_t val;

    print_current_ring();

    pointer = (uint8_t*)0x0abc;                 /* Any address below 2 MB */
    printk("Read a value from 0x%016x\n", pointer);

    val = *pointer;                             /* #PF is here */

    /* We shouldn't be here */
    printk("Value = %x\n", val);
}

void run_user_task_3(void) {
    print_title(3, "Try to access a supervisor page (#PF expected)");
    pause();

    /* No PAGE_USER flag; access is restricted to supervisor level (CPL < 3) */
    set_page_0_bits(PAGE_PRESENT | PAGE_WRITE | PAGE_PS /*| PAGE_USER */);

    /* Go to user mode */
    usermode_enter(&user_task_3);
    pause();
    printk("\n");
}

/*
 * Task 4. Tries to write a value to a read-only page.
 * When #PF exception occurs, the interrupt handler will make the page writable and return control.
 * As a result, the new value will be written.
 */
void user_task_4(void) {
    uint8_t* pointer;
    uint8_t val;

    pointer = (uint8_t*)0x1234;
    val = *pointer;                             /* Read the value */

    print_current_ring();
    printk("Write the value 2 to address 0x%016x [old value = %x]\n", pointer, val);

    *pointer = 2;                               /* Write some value; #PF is here */

    val = *pointer;                             /* Read again */

    print_current_ring();
    printk("Done. The value is written [new value = %x]\n", val);
}

void run_user_task_4(void) {
    uint8_t* pointer;

    print_title(4, "Try to write to a read-only page (#PF / continue execution)");
    pause();

    set_page_0_bits(PAGE_PRESENT | PAGE_WRITE | PAGE_PS);

    pointer = (uint8_t*)0x1234;                 /* Any address below 2 MB */
    *pointer = 1;

    /* Read-only page, no PAGE_WRITE flag */
    set_page_0_bits(PAGE_PRESENT | PAGE_USER | PAGE_PS /*| PAGE_WRITE */);

    /* Go to user mode */
    usermode_enter(&user_task_4);
    pause();
    printk("\n");
}
