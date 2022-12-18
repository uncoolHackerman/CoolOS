// kernel.c 04/12/2022
// Written by Gabriel Jickells

#include "stdio.h"
#include "memory.h"
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"

int __attribute__((section(".entry"))) main(uint8_t BootDrive) {
    memset(&__bss_start, 0, &__end - &__bss_start);             // clear uninitialised global variables
    CHAR_BACKGROUND = VGA_COLOUR_BLACK;
    CHAR_FOREGROUND = VGA_COLOUR_LIGHT_CYAN;
    ClrScr();
    printf("CoolOS Kernel v0.0.04 booted from drive 0%xh\n", BootDrive);
    printf("Kernel size (on disk): %u kilobytes (%u bytes in memory)\n", ((&__end - &phys) + 1023) / 1024, &__end - &phys);
    printf("Loading a new 32-bit Global Descriptor Table (GDT)\n");
    InitGDT();
    printf("g_GDT = 0x%p\n", g_GDT);
    printf("Loading a new Interrupt Descriptor Table (IDT)\n");
    InitIDT();
    printf("g_IDT = 0x%p\n", g_IDT);
    printf("Initialising Interrupt Service Routines (ISRs)\n");
    InitISR();
    printf("Initialising Interrupt Requests (IRQs)\n");
    InitIRQ();
    char c;
    KbdBuffer = 0;
    for(;;) {
        if(KbdBuffer) {
            putc(KbdBuffer);
            KbdBuffer = 0;
        }
    }
    return 0;
}