; stage2.asm 27/11/2022
; Written by Gabriel Jickells

[bits 16]

; To Do
; Enable the A20 line

[extern __bss_start]
[extern __end]
[extern main]

[global entry]
entry:
    mov byte [g_BootDrive], dl
    mov ah, 0x01                ; disable cursor
    mov ch, 0x3F
    int 0x10
    cli                         ; step 1: disable interrupts
    lgdt [GDT.Desc]             ; step 3: load GDT
    mov eax, cr0                ; step 4: enable the PE bit of cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x0008:.pmode       ; step 5: perform a far jump to 32 bit mode
    .pmode:
        [bits 32]
        mov eax, 0x10           ; step 6: set the segment registers to 32 bit data
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov esp, 0x0000FFF0
        mov ebp, esp
        mov edi, __bss_start    ; clear uninitialised variables 
        mov ecx, __end
        mov dword [g_HeapStart], ecx
        sub ecx, edi
        mov al, 0
        cld
        rep stosb
        xor edx, edx
        mov dl, [g_BootDrive]
        push edx
        call main
        hlt

[bits 16]
g_BootDrive: db 0
[global g_HeapStart]
g_HeapStart: dd 0

GDT:
    .Null:
        dq 0
    .Code32:
        dw 0xFFFF               ; limit low
        dw 0x0000               ; base low
        db 0x00                 ; base middle
        db 0b10011010           ; access byte
        db 0b11001111           ; (flags << 4) | limit high
        db 0                    ; base high
    .Data32:
        dw 0xFFFF               ; limit low
        dw 0x0000               ; base low
        db 0x00                 ; base middle
        db 0b10010010           ; access byte
        db 0b11001111           ; (flags << 4) | limit high
        db 0x00                 ; base high
    .Code16:
        dw 0xFFFF               ; limit low
        dw 0x0000               ; base low
        db 0x00                 ; base middle
        db 0b10011010           ; access byte
        db 0b00001111           ; (flags << 4) | limit high
        db 0x00                 ; base high
    .Data16:
        dw 0xFFFF               ; limit low
        dw 0x0000               ; base low
        db 0x00                 ; base middle
        db 0b10010010           ; access byte
        db 0b00001111           ; (flags << 4) | limit high
        db 0x00                 ; base high
    .Desc:
        dw .Desc - GDT - 1      ; Size
        dd GDT                  ; Offset

; uint8_t inb(uint16_t port)
global inb
inb:
    [bits 32]
    push ebp
    mov ebp, esp
    xor eax, eax
    mov edx, [ebp + 8]
    in al, dx
    mov esp, ebp
    pop ebp
    ret
global outb
outb:
    [bits 32]
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al
    mov esp, ebp
    pop ebp
    ret

%macro EnterRealMode 0
    jmp word 0x0018:.pmode16
    .pmode16:
        [bits 16]
        mov eax, cr0
        and al, ~1
        mov cr0, eax
        jmp word 0x0000:.rmode
    .rmode:
        xor ax, ax
        mov ds, ax
        mov ss, ax
        sti
%endmacro

%macro EnterProtectedMode 0
    cli
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x08:.pmode32
    .pmode32:
        [bits 32]
        mov ax, 0x0010
        mov ds, ax
        mov ss, ax
%endmacro

; -ARGUMENTS-
; %1 = linear address
; %2 = segment buffer (usually es)
; %3 = 32 bit offset buffer (e.g. eax)
; %4 = low 16 bits of %3 (e.g ax)
%macro LinearToSegment 4
    mov %3, %1                  ; load linear address into register %3
    shr %3, 4                   ; segment
    mov %2, %4
    mov %3, %1                  ; offset
    and %3, 0xf
%endmacro

global BIOS_ReadSectors
BIOS_ReadSectors:
    [bits 32]
    push ebp
    mov ebp, esp
    EnterRealMode
    push ebx
    push es
    mov dl, [bp + 8]            ; dl = drive
    mov ch, [bp + 12]           ; ch = cylinder low
    mov cl, [bp + 13]           ; cl = cylinder high
    shl cl, 6
    mov al, [bp + 20]           ; cl = sector
    and al, 0x3F
    or cl, al
    mov dh, [bp + 16]           ; dh = head
    mov al, [bp + 24]           ; al = count
    LinearToSegment [bp + 28], es, ebx, bx
    mov ah, 0x02
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0
    pop es
    pop ebx
    push eax
    EnterProtectedMode
    pop eax
    mov esp, ebp
    pop ebp
    ret

; to do
global BIOS_GetDriveParameters
BIOS_GetDriveParameters:
    [bits 32]
    push ebp
    mov ebp, esp
    EnterRealMode
    push es
    push di
    push bx
    xor di, di                      ; es:di = 0x0000:0x0000
    mov es, di
    mov dl, [bp + 8]                ; dl = drive
    mov ah, 0x08
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0
    LinearToSegment [bp + 12], es, esi, si
    mov byte [es:si], bl                 ; Drive type = bl
    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx                              ; Cylinders = bx
    LinearToSegment [bp + 16], es, esi, si
    mov word [es:si], bx
    xor ch, ch
    and cl, 0x3F                        ; Sectors per Track = cx
    LinearToSegment [bp + 24], es, esi, si
    mov word [es:si], cx
    mov cl, dh
    inc cx                              ; Heads = cx
    LinearToSegment [bp + 20], es, esi, si
    mov word [es:si], cx
    pop bx
    pop di
    pop es
    push eax
    EnterProtectedMode
    pop eax
    mov esp, ebp
    pop ebp
    ret

; to do
global BIOS_ResetDisk
BIOS_ResetDisk:
    push ebp
    mov ebp, esp
    EnterRealMode
    mov dl, [bp + 8]
    mov ah, 0x00
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0
    EnterProtectedMode
    mov esp, ebp
    pop ebp
    ret