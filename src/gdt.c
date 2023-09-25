/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "gdt.h"

#include <intrin.h>

__declspec(align(8))
gdt_t gdt = {
    .null       = 0,                            /* 0x08 null */
    .code       = 0x0020980000000000,           /* 0x08 kernel code [ DPL=0 ] */
    .data       = 0x0000920000000000,           /* 0x10 kernel data [ DPL=0 ] */
    .null_user  = 0,                            /* 0x18 user null */
    .user_data  = 0x0000f20000000000,           /* 0x20 user data [ DPL=3 ] */
    .user_code  = 0x0020f80000000000,           /* 0x28 user code [ DPL=3 ] */
    .tss_low    = 0x00a0890000000000,           /* 0x30 tss low, filled with init_tss function */
    .tss_high   = 0,                            /* 0x38 tss high, filled with init_tss function */
};

__declspec(align(8))
gdtr_t gdtr = {sizeof(gdt_t) - 1, (uint64_t)&gdt};

__declspec(align(8))
tss_t tss = {0};

#pragma pack(push, 1)

typedef struct {
    unsigned limit_0_15     : 16;
    unsigned base_0_15      : 16;
    unsigned base_16_23     : 8;
    unsigned type           : 4;
    unsigned                : 1;
    unsigned dpl            : 2;
    unsigned present        : 1;
    unsigned limit_16_19    : 4;
    unsigned available      : 1;
    unsigned                : 2;
    unsigned granularity    : 1;
    unsigned base_24_31     : 8;
    unsigned base_32_63     : 32;
    unsigned                : 32;
} tss_desc_t;

#pragma pack(pop)

#define KERNEL_STACK_SIZE 1024
uint8_t kernel_stack[KERNEL_STACK_SIZE];

void init_tss(void) {
    tss_desc_t tss_low = *((tss_desc_t*)&gdt.tss_low);
    tss_desc_t tss_high = *((tss_desc_t*)&gdt.tss_high);

    uint64_t tss_addr = ((uint64_t)&tss);

    /* Write the TSS address in the low/high descriptors */
    tss_low.base_0_15 = tss_addr & 0xffff;
    tss_low.base_16_23 = (tss_addr >> 16) & 0xff;
    tss_low.base_24_31 = (tss_addr >> 24) & 0xff;
    tss_low.limit_0_15 = sizeof(tss);

    tss_high.base_0_15 = (tss_addr >> 48) & 0xffff;
    tss_high.limit_0_15 = (tss_addr >> 32) & 0xffff;

    gdt.tss_low = *((int64_t*)&tss_low);
    gdt.tss_high = *((int64_t*)&tss_high);

    /* Location of the kernel stack */
    tss.rsp0 = (uint64_t)(kernel_stack + KERNEL_STACK_SIZE);
}

void setup_gdt(void) {
    init_tss();
    _lgdt(&gdtr);
}
