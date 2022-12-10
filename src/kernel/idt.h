// idt.h 08/12/2022
// Written by Gabriel Jickells

#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>

typedef struct IDT_Entry {
    uint16_t AddressLow;            // address of the interrupt handler
    uint16_t SegmentSelector;
    uint8_t _reserved;              // must always be zero
    uint8_t attributes;
    uint16_t AddressHigh;
} __attribute__((packed)) IDT_Entry;

typedef struct IDT_Desc {
    uint16_t size;                  // sizeof(IDT) - 1
    IDT_Entry* Address;
} __attribute__((packed)) IDT_Desc;

// to do: make enums for idt attributes

extern void LoadIDT(IDT_Desc* descriptor);
extern void InterruptHandler_asm(void);

#define IDT_ENTRIES 256

const char* g_Exceptions[] = {
    "divide by zero error",
    "debug",
    "NMI",
    "breakpoint",
    "overflow",
    "bound range exceeded",
    "invalid opcode",
    "device not available",
    "double fault",
    "co-processor segment overrun",
    "invalid tss",
    "segment not present",
    "stack segment fault",
    "general protection fault",
    "page fault",
    "reserved",
    "x87 floating point exception",
    "alignment check",
    "machine check",
    "SIMD floating point exception",
    "virtualisation exception",
    "control protection exception",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "hypervisor injection exception",
    "VMM communication exception",
    "security exception",
    "reserved"
};

IDT_Entry g_IDT[IDT_ENTRIES] = {0};
IDT_Desc g_IDTDesc = {sizeof(g_IDT) - 1, g_IDT};

void InitIDT(void) {
    for(int i = 0; i < IDT_ENTRIES; i++) {
        g_IDT[i].attributes = 0b10001110;
        g_IDT[i]._reserved = 0;
        g_IDT[i].SegmentSelector = 0x08;        // kernel code segment needs to be segment selector 0x08
        g_IDT[i].AddressHigh = (uint16_t)((uint32_t)InterruptHandler_asm >> 16);
        g_IDT[i].AddressLow = (uint16_t)((uint32_t)InterruptHandler_asm & 0x0000FFFF);
    }
    LoadIDT(&g_IDTDesc);
    //__asm("sti");     // gives a general protection fault with error code 000001001000 10 0
    return;
}

#endif