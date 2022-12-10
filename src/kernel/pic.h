// pic.h 09/12/2022
// Written by Gabriel Jickells

#ifndef _PIC_H_
#define _PIC_H_

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#include "io.h"

enum PIC_ICW1 {
    PIC_ICW1_ENABLE_ICW4 = 0x01,                // if set, PIC expects a fourth initialisation control word (ICW)
    PIC_ICW1_SINGLE = 0x02,                     // if set, PIC will not use the slave chip
    PIC_ICW1_LEVEL = 0x08,                      // if set, PIC will operate in level triggered mode
    PIC_ICW1_INITIALISE = 0x10                  // if set, PIC will start the initialisation sequence
} PIC_ICW1;

enum PIC_ICW4 {
    PIC_ICW4_8086 = 0x01,
    PIC_ICW4_AUTOEOI = 0x02
} PIC_ICW4;

enum PIC_CMD {
    PIC_EOI = 0x20,
    PIC_READ_IRR = 0x0A,
    PIC_READ_ISR = 0x0B,
} PIC_CMD;

void PIC_Remap(uint8_t PIC1Offset, uint8_t PIC2Offset) {
    outb(PIC1_COMMAND, PIC_ICW1_ENABLE_ICW4 | PIC_ICW1_INITIALISE);     // ICW1: enable the initialisation sequence
    io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_ENABLE_ICW4 | PIC_ICW1_INITIALISE);
    io_wait();
    outb(PIC1_DATA, PIC1Offset);                // ICW2: tell the PICs which ISRs they will use
    io_wait();
    outb(PIC2_DATA, PIC2Offset);
    io_wait();
    outb(PIC1_DATA, 0x04);                      // ICW3: tell PIC1 that it has a slave at IRQ 2
    io_wait();
    outb(PIC2_DATA, 0x02);                      // tell PIC2 where on PIC1 it is connected
    io_wait();
    outb(PIC1_DATA, PIC_ICW4_8086 | PIC_ICW4_AUTOEOI);          // ICW4
    io_wait();
    outb(PIC1_DATA, PIC_ICW4_8086 | PIC_ICW4_AUTOEOI);
    io_wait();
    outb(PIC1_DATA, 0);                     // clear the data ports
    io_wait();
    outb(PIC2_DATA, 0);
    io_wait();
    return;
}

void PIC_SetMask(unsigned int irq) {
    if(irq < 8) {
        uint8_t mask = inb(PIC1_DATA) | (1 << irq);
        outb(PIC1_DATA, mask);
    }
    else {
        irq -= 8;
        uint8_t mask = inb(PIC2_DATA) | (1 << irq);
        outb(PIC2_DATA, mask);
    }
    return;
}

void PIC_ClearMask(unsigned int irq) {
    if(irq < 8) {
        uint8_t mask = inb(PIC1_DATA) & ~(1 << irq);
        outb(PIC1_DATA, mask);
    }
    else {
        irq -= 8;
        uint8_t mask = inb(PIC2_DATA) & ~(1 << irq);
        outb(PIC2_DATA, mask);
    }
    return;
}

void PIC_Disable() {
    outb(PIC1_DATA, 0xFF);
    io_wait();
    outb(PIC2_DATA, 0xFF);
    io_wait();
    return;
}

void PIC_SendEOI(unsigned int irq) {
    if(irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
    return;
}

uint16_t PIC_GetIRR() {
    outb(PIC1_COMMAND, PIC_READ_IRR);
    outb(PIC2_COMMAND, PIC_READ_IRR);
    return (inb(PIC2_COMMAND) << 8) | (inb(PIC1_COMMAND));
}

uint16_t PIC_GetISR() {
    outb(PIC1_COMMAND, PIC_READ_ISR);
    outb(PIC2_COMMAND, PIC_READ_ISR);
    return (inb(PIC2_COMMAND) << 8) | (inb(PIC1_COMMAND));
}

#endif