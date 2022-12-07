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
    printf("CoolOS Kernel v0.0.01 booted from drive 0%xh\n", BootDrive);
    for(;;);            // don't return to stage2.bin
}