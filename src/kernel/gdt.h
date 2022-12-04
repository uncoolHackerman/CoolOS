// gdt.h 04/12/2022
// Written by Gabriel Jickells

#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>

typedef struct GdtEntry {
    uint16_t LimitLow;
    uint16_t BaseLow;
    uint8_t BaseMid;
    uint8_t Access;
    uint8_t Flags_LimitHigh;            // (flags << 4) | Limit High 4 bits
    uint16_t BaseHigh;
} __attribute__((packed)) GdtEntry;

typedef struct GdtDescriptor {
    uint16_t Size;                      // sizeof(gdt) - 1
    uint32_t Address;
} __attribute__((packed)) GdtDescriptor;

enum GDT_CODE_ACCESS {
    GDT_CODE_READABLE       = 0b00000010,
    GDT_CODE_CONFORMING     = 0b00000100,
    GDT_CODE_SEGMENT        = 0b00011000,
};

enum GDT_DATA_ACCESS {
    GDT_DATA_WRITABLE       = 0b00000010,
    GDT_DATA_GROW_UP        = 0b00000100,
    GDT_DATA_GROW_DOWN      = 0b00000000,
    GDT_DATA_SEGMENT        = 0b00010000,
};

enum GDT_ACCESS {
    GDT_RING0               = 0b00100000,
    GDT_RING1               = 0b01000000,
    GDT_RING2               = 0b01000000,
    GDT_RING3               = 0b01100000,
    GDT_PRESENT             = 0b10000000
};

enum GDT_SYSTEM_ACCESS {
    GDT_SYSTEM_TSS16_FREE   = 0b00000001,
    GDT_SYSTEM_LDT          = 0b00000010,
    GDT_SYSTEM_TSS16_USED   = 0b00000011,
    GDT_SYSTEM_TSS32_FREE   = 0b00001001,
    GDT_SYSTEM_TSS32_USED   = 0b00001011,
};

enum GDT_FLAGS {
    GDT_MODE64              = 0b00100000,
    GDT_MODE32              = 0b01000000,
    GDT_MODE16              = 0b00000000,
    GDT_BLOCKS_4K           = 0b10000000,
    GDT_BLOCKS_1B           = 0b00000000
};

extern void LoadGDT(GdtDescriptor GDT_Desc);

#endif