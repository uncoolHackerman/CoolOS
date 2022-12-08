// gdt.h 04/12/2022
// Written by Gabriel Jickells

#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>
#include "memory.h"

typedef struct GdtEntry {
    uint16_t LimitLow;
    uint16_t BaseLow;
    uint8_t BaseMid;
    uint8_t Access;
    uint8_t Flags_LimitHigh;            // (flags << 4) | Limit High 4 bits
    uint8_t BaseHigh;
} __attribute__((packed)) GdtEntry;

typedef struct GdtDescriptor {
    uint16_t Size;                      // sizeof(gdt) - 1
    GdtEntry* Address;
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
    GDT_RING0               = 0b00000000,
    GDT_RING1               = 0b00100000,
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

extern void LoadGDT(GdtDescriptor* GDT_Desc, uint8_t CodeSegIndex, uint8_t DataSegIndex);

GdtEntry* g_GDT = NULL;
GdtDescriptor* g_GDTDesc;

#define G_GDT_ENTRIES 3         // includes the null descriptor

// will load a 32-bit kernel code segment, and a 32-bit kernel data segment
void InitGDT(void) {
    g_GDT = (GdtEntry*)malloc(sizeof(GdtEntry) * G_GDT_ENTRIES);
    g_GDT[0].LimitLow = 0;    // null descriptor
    g_GDT[0].BaseLow = 0;
    g_GDT[0].BaseMid = 0;
    g_GDT[0].Access = 0;
    g_GDT[0].Flags_LimitHigh = 0;
    g_GDT[0].BaseHigh = 0;
    g_GDT[1].LimitLow = 0xFFFF; // code32 descriptor
    g_GDT[1].BaseLow = 0x0000;
    g_GDT[1].BaseMid = 0x00;
    g_GDT[1].Access = GDT_PRESENT | GDT_RING0 | GDT_CODE_SEGMENT | GDT_CODE_READABLE;
    g_GDT[1].Flags_LimitHigh = GDT_BLOCKS_4K | GDT_MODE32 | 0x0F;
    g_GDT[1].BaseHigh = 0x00;
    g_GDT[2].LimitLow = 0xFFFF;    // data32 descriptor
    g_GDT[2].BaseLow = 0x0000;
    g_GDT[2].BaseMid = 0x00;
    g_GDT[2].Access = GDT_PRESENT | GDT_RING0 | GDT_DATA_SEGMENT | GDT_DATA_WRITABLE;
    g_GDT[2].Flags_LimitHigh = GDT_BLOCKS_4K | GDT_MODE32 | 0x0F;
    g_GDT[2].BaseHigh = 0x00;
    g_GDTDesc->Size = (sizeof(GdtEntry) * G_GDT_ENTRIES) - 1;
    g_GDTDesc->Address = g_GDT;
    LoadGDT(g_GDTDesc, 1, 2);
    return;
}

#endif