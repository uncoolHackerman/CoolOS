// irq.h 09/12/2022
// Written by Gabriel Jickells

#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdint.h>
#include "io.h"

// I/O addresses
#define PIC1 0x20           // master PIC i/o port
#define PIC2 0xA0           // slave PIC i/o port
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC1 + 1)

// Commands
#define PIC_EOI 0x20

void PIC_SendEOI(uint8_t irq) {
    if(irq >= 8)            // if the IRQ number is larger than the amount of IRQ lines on PIC1
        outb(PIC2, PIC_EOI);    // send the EOI instruction to PIC2
    outb(PIC1, PIC_EOI);        // EOI still needs to be sent to PIC1
    return;
}

#define ICW4_ENABLE 0x01
#define ICW1_CASCADE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

void PIC_Remap(int PIC1offset, int PIC2offset) {
    uint8_t masks1, masks2;
    masks1 = inb(PIC1_DATA);            // save masks already on the PICs
    masks2 = inb(PIC2_DATA);
    outb(PIC1_COMMAND, ICW1_INIT | ICW4_ENABLE); // start the initialisation sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW4_ENABLE);
    io_wait();
    outb(PIC1_DATA, PIC1offset);
    io_wait();
    outb(PIC2_DATA, PIC2offset);
    io_wait();
    outb(PIC1_DATA, 4);             // tell master PIC that there is a slave at irq2
    io_wait();
    outb(PIC2_DATA, 2);             // tell the slave PIC its cascade identity
    io_wait();
    outb(PIC1_DATA, 1);             // initialise in 8086/8088 mode
    io_wait();
    outb(PIC2_DATA, 1);
    io_wait();
    outb(PIC1_DATA, masks1);            // restore saved masks
    outb(PIC2_DATA, masks2);
    return;
}

#endif