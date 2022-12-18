/*
stage2.s 12/12/2022
Written by Gabriel Jickells
*/

.code16

.extern __bss_start
.extern __end
.extern main

.globl entry
entry:
    movb %dl, g_BootDrive
    movb $0x01, %ah             /* disable cursor */
    movb $0x3F, %ch
    int $0x10
    cli
    lgdt (GDT_Desc)
    movl %cr0, %eax
    orb $0x01, %al
    movl %eax, %cr0
    ljmp $0x08, $_pmode
    _pmode:
        .code32
        movl $0x10, %eax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss
        movl $0x0000FFF0, %esp
        movl %esp, %ebp
        movl $__bss_start, %edi
        movl $__end, %ecx
        movl %ecx, (g_HeapStart)
        subl %edi, %ecx
        movb $0, %al
        cld
        rep stosb
        xorl %edx, %edx
        mov (g_BootDrive), %dl
        pushl %edx
        call main
        hlt

.code16
g_BootDrive: .byte 0
.globl g_HeapStart
g_HeapStart: .long 0

GDT:
    GDT_Null:
        .word 0x0000
        .word 0x0000
        .byte 0x00
        .byte 0b00000000
        .byte 0b00000000
        .byte 0x00
    GDT_Code32:                         /* GDT Entry 0x08 */
        .word 0xFFFF                    /* limit low */
        .word 0x0000                    /* base low */
        .byte 0x00                      /* base middle */
        .byte 0b10011010                /* access byte */
        .byte 0b11001111                /* (flags << 4) | limit high */
        .byte 0x00                      /* base high */
    GDT_Data32:                         /* GDT Entry 0x10 */
        .word 0xFFFF                    /* limit low */
        .word 0x0000                    /* base low */
        .byte 0x00                      /* base middle */
        .byte 0b10010010                /* access byte */
        .byte 0b11001111                /* (flags << 4) | limit high */
        .byte 0x00                      /* base high */
    GDT_Code16:                         /* GDT Entry 0x08 */
        .word 0xFFFF                    /* limit low */
        .word 0x0000                    /* base low */
        .byte 0x00                      /* base middle */
        .byte 0b10011010                /* access byte */
        .byte 0b00001111                /* (flags << 4) | limit high */
        .byte 0x00                      /* base high */
    GDT_Data16:                         /* GDT Entry 0x10 */
        .word 0xFFFF                    /* limit low */
        .word 0x0000                    /* base low */
        .byte 0x00                      /* base middle */
        .byte 0b10010010                /* access byte */
        .byte 0b00001111                /* (flags << 4) | limit high */
        .byte 0x00                      /* base high */
    GDT_Desc:
        .word GDT_Desc - GDT - 1        /* Size */
        .long GDT                       /* Offset */
    
/* uint8_t inb(uint16_t port) */
.globl inb
inb:
    .code32
    pushl %ebp
    movl %esp, %ebp
    xorl %eax, %eax
    movw 8(%ebp), %dx
    inb %dx, %al
    movl %ebp, %esp
    popl %ebp
    ret

.globl outb
outb:
    .code32
    pushl %ebp
    movl %esp, %ebp
    movw (%ebp), %dx
    movl 12(%ebp), %eax
    outb %al, %dx
    movl %ebp, %esp
    popl %ebp
    ret

.macro EnterRealMode
    ljmp $0x18, $1f
    1:
        .code16
        movl %cr0, %eax
        andb $0xFE, %al
        movl %eax, %cr0
        ljmp $0x00, $2f
    2:
        xorw %ax, %ax
        movw %ax, %ds
        movw %ax, %ss
        sti
.endm

.macro EnterProtectedMode
    cli
    movl %cr0, %eax
    orb $1, %al
    movl %eax, %cr0
    ljmp $0x08, $1f
    1:
        .code32
        movw $0x10, %ax
        movw %ax, %ds
        movw %ax, %ss
.endm

/*
-ARGUMENTS-
%1 = linear address
%2 = segment buffer (usually es)
%3 = 32 bit offset buffer (e.g. eax)
%4 = low 16 bits of %3 (e.g ax)
*/
.macro LinearToSegment lin_addr, seg_buffer, offset_buffer, offset_buffer16
    movl \lin_addr, \offset_buffer                 /* load linear address into register %3 */
    shrl $4, \offset_buffer                     /* segment */
    movw \offset_buffer16, \seg_buffer
    movl \lin_addr, \offset_buffer              /* offset */
    andw $0xf, \offset_buffer16
.endm

.globl BIOS_ReadSectors
BIOS_ReadSectors:
    .code32
    pushl %ebp
    movl %esp, %ebp
    EnterRealMode
    pushl %ebx
    pushw %es
    movb 8(%bp), %dl             /* dl = drive */
    movb 12(%bp), %ch            /* ch = cylinder low */
    movb 13(%bp), %cl            /* cl = cylinder high */
    shlb $6, %cl
    movb 20(%bp), %al            /* cl = sector */
    andb $0x3f, %al
    orb %al, %cl
    movb 16(%bp), %dh            /* dh = head */
    movb 24(%bp), %al            /* al = count */
    LinearToSegment 28(%bp), %es, %ebx, %bx
    movb $0x02, %ah
    stc
    int $0x13
    movl $1, %eax
    sbbl $0, %eax
    popw %es
    popl %ebx
    pushl %eax
    EnterProtectedMode
    pop %eax
    mov %ebp, %esp
    pop %ebp
    ret

.globl BIOS_GetDriveParameters
BIOS_GetDriveParameters:
    .code32
    pushl %ebp
    movl %esp, %ebp
    EnterRealMode
    pushw %es
    pushw %di
    pushw %bx
    xorw %di, %di
    movw %di, %es
    mov 8(%bp), %dl
    mov $0x08, %ah
    stc
    int $0x13
    mov $1, %eax
    sbb $0, %eax
    LinearToSegment 12(%bp), %es, %esi, %si
    movb %bl, %es:(%si)
    movb %ch, %bl
    movb %cl, %bh
    shrb $6, %bh
    incw %bx
    LinearToSegment 16(%bp), %es, %esi, %si
    movw %bx, %es:(%si)
    xorb %ch, %ch
    andb $0x3F, %cl
    LinearToSegment 24(%bp), %es, %esi, %si
    movw %cx, %es:(%si)
    movb %dh, %cl
    incw %cx
    LinearToSegment 20(%bp), %es, %esi, %si
    movw %cx, %es:(%si)
    popw %bx
    popw %di
    popw %es
    pushl %eax
    EnterProtectedMode
    popl %eax
    movl %ebp, %esp
    popl %ebp
    ret

.globl BIOS_ResetDisk
BIOS_ResetDisk:
    pushl %ebp
    movl %esp, %ebp
    EnterRealMode
    movb 8(%bp), %dl
    movb $0x00, %ah
    stc
    int $0x13
    mov $1, %eax
    sbb $0, %eax
    EnterProtectedMode
    mov %ebp, %esp
    pop %ebp
    ret
