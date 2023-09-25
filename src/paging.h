/* (c) 2023 Max Kolesnikov
 * This code is licensed under the GPL 2.0 license, available at the root
 * application directory.
 */

#ifndef PAGING_H
#define PAGING_H

#include <efi.h>

#define PAGE_PRESENT            (1 << 0)
#define PAGE_WRITE              (1 << 1)
#define PAGE_USER               (1 << 2)
#define PAGE_PS                 (1 << 7)

/* Memory range */
#define MIN_PHYSICAL_ADDRESS    0
#define MAX_PHYSICAL_ADDRESS    (0x8000000000 - 1)

/* 2-Mbyte physical page is used */
#define PAGE_SIZE_2M            0x200000

/* Sets 1:1 page mapping in the range 0-0x8000000000 */
void setup_identity_paging(void);

/* Helper function that changes the attributes of the first memory page */
void set_page_0_bits(uint8_t bits);

#endif /* PAGING_H */
