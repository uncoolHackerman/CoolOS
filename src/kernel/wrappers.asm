; wrappers.asm 04/12/2022
; Written by Gabriel Jickells

; void LoadGDT(GdtDescriptor GDT_Desc)
[bits 32]

global LoadGDT
LoadGDT:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    lgdt [eax]
    mov eax, [ebp + 12]
    shl eax, 3              ; eax *= 8 (sizeof(GdtEntry))
    push eax
    push .gdtLoaded
    retf                    ; jmp far [ss:esp + 1]:[ss:esp] (reloads code segment)
    .gdtLoaded:             ; reload other segment registers
        mov ax, [ebp + 16]
        shl ax, 3
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
    mov esp, ebp
    pop ebp
    ret