// kernel.c 04/12/2022
// Written by Gabriel Jickells

#include "stdio.h"
#include "memory.h"
#include <stdint.h>

int __attribute__((section(".entry"))) main(uint8_t BootDrive) {
    memset(&__bss_start, 0, &__end - &__bss_start);             // clear uninitialised global variables
    ClrScr();
    printf("CoolOS Kernel booted from drive 0%xh\n", BootDrive);
    for(;;);
}