/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#include "paging.h"

#include <intrin.h>

/* Page-map level-4 table */
__declspec(align(4096))
uint64_t pg_pml4[512] = {0};

/* Page-directory-pointer table */
__declspec(align(4096))
uint64_t pg_pdp[512];

/* Page-directory table */
__declspec(align(4096))
uint64_t pg_pd[512 * 512];

#define PD_TABLE_SIZE (512 * sizeof(uint64_t))

/*
 * Fills memory pages for the range 0-0x8000000000
 * Page size: 2MB
 */
void init_identity_paging_2M(void) {
    uint64_t paddr;                             /* Physical address */

    /* 1 entry in PML4 */
    pg_pml4[0] = ((uint64_t)&pg_pdp) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;


    /* 512 entries in PDP */
    for (int i = 0; i < 512; i++) {
        pg_pdp[i] = (((uint64_t)&pg_pd) + (PD_TABLE_SIZE * i)) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }

    /*
     * 512 * 512: 512 entries for each entry in PDP
     * Max physical address = 512 * 512 * 0x200000 = 0x80'0000'0000
     */
    paddr = 0;
    for (int i = 0; i < 512 * 512; i++) {
        pg_pd[i] = paddr | PAGE_PRESENT | PAGE_WRITE | PAGE_USER | PAGE_PS;
        paddr += PAGE_SIZE_2M;
    }
}

/* Sets 1:1 page mapping in the range 0-0x8000000000 */
void setup_identity_paging(void) {
    init_identity_paging_2M();
    __writecr3((uint64_t)&pg_pml4);
}

/* Helper function that changes the attributes of the first memory page */
void set_page_0_bits(uint8_t bits) {
    /* Page-directory table */
    pg_pd[0] = 0x0 | bits;
    __invlpg(0);
}
