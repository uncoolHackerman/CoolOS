// irq.h 10/12/2022
// Written by Gabriel Jickells

#ifndef _IRQ_H_
#define _IRQ_H_

#include "isr.h"
#include "pic.h"

Interrupt_Handler g_IRQ_Handlers[16];

#define PIC_OFFSET 0x20

void IRQ_Handler(Interrupt_Registers* registers) {
    int irq = registers->IntNum - PIC_OFFSET;
    if(g_IRQ_Handlers[irq])
        g_IRQ_Handlers[irq](registers);
    else
        printf("The CPU encountered an unhandled IRQ: irq 0x%x\n", irq);
    PIC_SendEOI(irq);
    return;
}

void InitIRQ() {
    PIC_Remap(PIC_OFFSET, PIC_OFFSET + 8);
    for(int i = 0; i < 16; i++)
        g_ISR_Handlers[PIC_OFFSET + i] = IRQ_Handler;
    x86_sti();
    return;
}

#endif