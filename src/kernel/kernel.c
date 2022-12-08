// kernel.c 04/12/2022
// Written by Gabriel Jickells

#include "stdio.h"
#include "memory.h"
#include <stdint.h>
#include "gdt.h"


int __attribute__((section(".entry"))) main(uint8_t BootDrive) {
    memset(&__bss_start, 0, &__end - &__bss_start);             // clear uninitialised global variables
    CHAR_BACKGROUND = VGA_COLOUR_BLACK;
    CHAR_FOREGROUND = VGA_COLOUR_LIGHT_CYAN;
    ClrScr();
    printf("CoolOS Kernel v0.0.02 booted from drive 0%xh\n", BootDrive);
    printf("Loading a new 32-bit only GDT\n");
    InitGDT();
    printf("g_GDT = 0x%p\n", g_GDT);
    return 0;
}