// kernel.c 04/12/2022
// Written by Gabriel Jickells

#include "stdio.h"
#include "memory.h"
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "irq.h"

void timerHandler(Interrupt_Registers registers) {
    
    putc('.');
    return;
}

char g_KbMap[] = 
{
    0, 1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '_', '=', 0x08,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 'L', '#',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'R', '*', 
    0, ' ', 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'N', 'S', 
    '7', '8', '9', '-',
    '4', '5', '6', '+',
    '1', '2', '3', 0, 0, 0, 0, 
    '\\', 0, 0 //0x58
};

void keyboardHandler(Interrupt_Registers registers) {
    char c = inb(0x60);
    if(!(c & 0x80) && c && g_KbMap[c])
        putc(g_KbMap[c]);
    return;
}

int __attribute__((section(".entry"))) main(uint8_t BootDrive) {
    memset(&__bss_start, 0, &__end - &__bss_start);             // clear uninitialised global variables
    CHAR_BACKGROUND = VGA_COLOUR_BLACK;
    CHAR_FOREGROUND = VGA_COLOUR_LIGHT_CYAN;
    ClrScr();
    printf("CoolOS Kernel v0.0.03 booted from drive 0%xh\n", BootDrive);
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
    g_IRQ_Handlers[0] = (Interrupt_Handler)timerHandler;
    g_IRQ_Handlers[1] = (Interrupt_Handler)keyboardHandler;
    for(;;);
    return 0;
}