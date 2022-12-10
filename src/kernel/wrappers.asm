; wrappers.asm 04/12/2022
; Written by Gabriel Jickells
[bits 32]

global LoadGDT
LoadGDT:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    lgdt [eax]
    mov eax, [ebp + 12]
    shl eax, 3              ; eax *= 8 (8 =  sizeof(GdtEntry))
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

global LoadIDT
LoadIDT:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    lidt [eax]
    mov esp, ebp
    pop ebp
    ret

global InterruptHandler_asm
InterruptHandler_asm:                   ; the default if the isr has not been initialised
    iret                                ; iret instruction to return from an interrupt

extern ISR_Handler

ISR_Common:
    pusha                               ; push eax, ecx, edx, ebx, esp, ebp, esi, edi
    xor eax, eax                        ; push ds
    mov ax, ds
    push eax
    mov ax, 0x10                        ; use kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call ISR_Handler
    add esp, 4
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8                          ; remove error code and interrupt number
    iret

global inb
inb:
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
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al
    mov esp, ebp
    pop ebp
    ret

global x86_cli
x86_cli:
    cli
    ret

global x86_sti
x86_sti:
    sti
    ret

%macro isr_noerr 1
    global ISR_NOERR_%1
    ISR_NOERR_%1:
        push 0          ; dummy error code
        push %1         ; interrupt number
        jmp ISR_Common  ; got to the actual interrupt handler
%endmacro

%macro isr_err 1
    global ISR_ERR_%1
    ISR_ERR_%1:
        ; error code will be already on the stack
        push %1         ; interrupt number
        jmp ISR_Common
%endmacro

isr_noerr 0             ; divide by zero error
isr_noerr 1             ; debug exception
isr_noerr 2             ; Non Maskable Interrupt (NMI)
isr_noerr 3             ; breakpoint
isr_noerr 4             ; overflow
isr_noerr 5             ; bound range exceeded
isr_noerr 6             ; invalid opcode
isr_noerr 7             ; device not available
isr_err 8               ; double fault
isr_noerr 9             ; co-processor segment overrun
isr_err 10              ; invalid tss
isr_err 11              ; segment not present
isr_err 12              ; stack segment fault
isr_err 13              ; general protection fault
isr_err 14              ; page fault
isr_noerr 15
isr_noerr 16            ; x87 floating point exception
isr_err 17              ; alignment check
isr_noerr 18            ; machine check
isr_noerr 19            ; SIMD floating point exception
isr_noerr 20            ; virtualisation exception
isr_err 21              ; control protection exception
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28            ; hypervisor injection exception
isr_err 29              ; VMM commumication exception
isr_err 30              ; security exception
isr_noerr 31
isr_noerr 32
isr_noerr 33
isr_noerr 34
isr_noerr 35
isr_noerr 36
isr_noerr 37
isr_noerr 38
isr_noerr 39
isr_noerr 40
isr_noerr 41
isr_noerr 42
isr_noerr 43
isr_noerr 44
isr_noerr 45
isr_noerr 46
isr_noerr 47
isr_noerr 48
isr_noerr 49
isr_noerr 50
isr_noerr 51
isr_noerr 52
isr_noerr 53
isr_noerr 54
isr_noerr 55
isr_noerr 56
isr_noerr 57
isr_noerr 58
isr_noerr 59
isr_noerr 60
isr_noerr 61
isr_noerr 62
isr_noerr 63
isr_noerr 64
isr_noerr 65
isr_noerr 66
isr_noerr 67
isr_noerr 68
isr_noerr 69            ; nice
isr_noerr 70
isr_noerr 71
isr_noerr 72
isr_noerr 73
isr_noerr 74
isr_noerr 75
isr_noerr 76
isr_noerr 77
isr_noerr 78
isr_noerr 79
isr_noerr 80
isr_noerr 81
isr_noerr 82
isr_noerr 83
isr_noerr 84
isr_noerr 85
isr_noerr 86
isr_noerr 87
isr_noerr 88
isr_noerr 89
isr_noerr 90
isr_noerr 91
isr_noerr 92
isr_noerr 93
isr_noerr 94
isr_noerr 95
isr_noerr 96
isr_noerr 97
isr_noerr 98
isr_noerr 99
isr_noerr 100
isr_noerr 101
isr_noerr 102
isr_noerr 103
isr_noerr 104
isr_noerr 105
isr_noerr 106
isr_noerr 107
isr_noerr 108
isr_noerr 109
isr_noerr 110
isr_noerr 111
isr_noerr 112
isr_noerr 113
isr_noerr 114
isr_noerr 115
isr_noerr 116
isr_noerr 117
isr_noerr 118
isr_noerr 119
isr_noerr 120
isr_noerr 121
isr_noerr 122
isr_noerr 123
isr_noerr 124
isr_noerr 125
isr_noerr 126
isr_noerr 127
isr_noerr 128
isr_noerr 129
isr_noerr 130
isr_noerr 131
isr_noerr 132
isr_noerr 133
isr_noerr 134
isr_noerr 135
isr_noerr 136
isr_noerr 137
isr_noerr 138
isr_noerr 139
isr_noerr 140
isr_noerr 141
isr_noerr 142
isr_noerr 143
isr_noerr 144
isr_noerr 145
isr_noerr 146
isr_noerr 147
isr_noerr 148
isr_noerr 149
isr_noerr 150
isr_noerr 151
isr_noerr 152
isr_noerr 153
isr_noerr 154
isr_noerr 155
isr_noerr 156
isr_noerr 157
isr_noerr 158
isr_noerr 159
isr_noerr 160
isr_noerr 161
isr_noerr 162
isr_noerr 163
isr_noerr 164
isr_noerr 165
isr_noerr 166
isr_noerr 167
isr_noerr 168
isr_noerr 169
isr_noerr 170
isr_noerr 171
isr_noerr 172
isr_noerr 173
isr_noerr 174
isr_noerr 175
isr_noerr 176
isr_noerr 177
isr_noerr 178
isr_noerr 179
isr_noerr 180
isr_noerr 181
isr_noerr 182
isr_noerr 183
isr_noerr 184
isr_noerr 185
isr_noerr 186
isr_noerr 187
isr_noerr 188
isr_noerr 189
isr_noerr 190
isr_noerr 191
isr_noerr 192
isr_noerr 193
isr_noerr 194
isr_noerr 195
isr_noerr 196
isr_noerr 197
isr_noerr 198
isr_noerr 199
isr_noerr 200
isr_noerr 201
isr_noerr 202
isr_noerr 203
isr_noerr 204
isr_noerr 205
isr_noerr 206
isr_noerr 207
isr_noerr 208
isr_noerr 209
isr_noerr 210
isr_noerr 211
isr_noerr 212
isr_noerr 213
isr_noerr 214
isr_noerr 215
isr_noerr 216
isr_noerr 217
isr_noerr 218
isr_noerr 219
isr_noerr 220
isr_noerr 221
isr_noerr 222
isr_noerr 223
isr_noerr 224
isr_noerr 225
isr_noerr 226
isr_noerr 227
isr_noerr 228
isr_noerr 229
isr_noerr 230
isr_noerr 231
isr_noerr 232
isr_noerr 233
isr_noerr 234
isr_noerr 235
isr_noerr 236
isr_noerr 237
isr_noerr 238
isr_noerr 239
isr_noerr 240
isr_noerr 241
isr_noerr 242
isr_noerr 243
isr_noerr 244
isr_noerr 245
isr_noerr 246
isr_noerr 247
isr_noerr 248
isr_noerr 249
isr_noerr 250
isr_noerr 251
isr_noerr 252
isr_noerr 253
isr_noerr 254
isr_noerr 255