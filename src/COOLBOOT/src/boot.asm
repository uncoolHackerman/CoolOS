; boot.asm 23/11/2022
; Written by Gabriel Jickells

[org 0x7C00]
[bits 16]

; BIOS Parameter Block
jmp short start
nop
bpb_Identifier: db "COOLBOOT"   ; OEM Identifier, 8 bytes
bpb_BytesPerSector: dw 512      ; Bytes Per Sector, 2 bytes
bpb_SectorsPerCluster: db 2     ; Sectors Per Cluster, 1 byte
bpb_ReservedSectors: dw 1       ; Reserved Sectors, 2 bytes
bpb_FatCount: db 2              ; Total FAT Count, 1 byte
bpb_RootDirEntries: dw 224      ; Total Root Directory Entries obtained from mkfs.fat, 2 bytes
bpb_TotalSectors: dw 2880       ; Total Sectors on the Disk, 2 bytes
bpb_MediaDescriptor: db 0xF0    ; Media Descriptor Type, 1 byte
bpb_SectorsPerFAT: dw 9         ; Sectors Per FAT, 2 bytes
bpb_SectorsPerTrack: dw 18      ; Sectors Per Cylinder, 2 bytes
bpb_HeadCount: dw 2             ; Number of Sides on the Disk, 2 bytes
bpb_HiddenSectors: dd 0         ; Number of Hidden Sectors / LBA of the Start of the partition, 4 bytes
bpb_LargeSectors: dd 0          ; Number of Large Sectors, 4 bytes
ebr_DriveNumber: db 00h         ; BIOS Drive Number, 1 byte
ebr_WinNTFlags: db 0            ; Windows NT Flags / Reserved, 1 byte
ebr_Signature: db 0x28          ; FAT Signature. Must be 0x28 or 0x29, 1 byte
ebr_SerialNumber: db "GJv0"     ; Serial Number of the Disk. Not important, 1 byte
ebr_VolumeID: db "GJ.COOLBOOT"  ; Volume ID, 11 bytes
ebr_SystemID: db "FAT12   "     ; System Identifier. Don't trust it, 8 bytes

; set registers
start:
    xor ax, ax                  ; reset the segment registers
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00              ; set up the stack
    mov bp, sp
    jmp 0x0000:main             ; reset the CS register

; -ARGUMENTS-
; si = address of string
puts:
    pusha                       ; push ax, cx, dx, bx, sp, bp, si, and di in that order
    mov ah, 0x0E                ; teletype output
    xor bh, 0                   ; page 0
    .loop:
        lodsb                   ; load ds:[si] into al and increment si
        test al, al             ; check for end of string
        jz .end
        int 0x10                ; BIOS video services
        jmp .loop
    .end:
        popa
        ret

; -ARGUMENTS-
; ax = LBA address
; -OUTPUT-
; ch = low 8 bits of cylinder number
; cl = sector number (bits 0 - 5) and high 2 bits of cylinder number (bits 6 - 7)
; dh = head number
LBA2CHS:
    push ax
    push dx    
    xor dx, dx
    div word [bpb_SectorsPerTrack]  ; ax = lba / sectorsPerTrack
    mov cl, dl                      ; cl = lba % sectorsPerTrack
    inc cx                          ; Sector Number
    xor dx, dx
    div word [bpb_HeadCount]        ; ax = (lba / sectorsPerTrack) / heads
    mov ch, al                      ; Cylinder Low
    shl ah, 6
    or cl, ah                       ; Cylinder High
    mov al, dl
    pop dx
    mov dh, al                      ; Head Number
    pop ax
    ret

; -ARGUMENTS-
; ax = LBA Address
; es:bx = output buffer
; cl = count
; dl = drive
ReadSectors:
    pusha
    push cx
    call LBA2CHS
    pop ax
    mov di, 3
    .retry:
        test di, di
        jz .fail
        dec di
        mov ah, 0x02
        stc                             ; error handling B)
        int 0x13
        jc .retry
        jmp .end
    .fail:
        jmp 0xF000:0xFFF0       ; start of the BIOS according to bochs
    .end:
        popa
        ret

; -ARGUMENTS-
; ax = cluster
; -OUTPUT-
; ax = LBA
Cluster2LBA:
    push dx
    sub ax, 2
    mul byte [bpb_SectorsPerCluster]
    add ax, [g_DataSectionLBA]
    pop dx
    ret

MSG_Loading: db "COOLBOOT v0.0.03", 0x0A, 0x0D, 0
g_File: db "STAGE2  BIN"                    ; DirectoryEntry* g_File
;_File: db "123456789AB"
g_DataSectionLBA: dw 0

main:
    mov byte [ebr_DriveNumber], dl
    xor di, di                              ; es:di = 0x0000:0x0000
    mov es, di
    mov dl, [ebr_DriveNumber]               ; dl = drive
    mov ah, 0x08                            ; get drive parameters
    stc
    int 0x13                                ; BIOS disk functions
    jc ReadSectors.fail
    xor ch, ch
    and cl, 0x3F
    mov word [bpb_SectorsPerTrack], cx      ; cx = Sectors per Track
    mov dl, dh
    xor dh, dh
    inc dx
    mov word [bpb_HeadCount], dx
    xor di, di
    mov es, di
    mov si, MSG_Loading
    call puts
    .ReadRootDirectory:
        xor ax, ax
        mov al, [bpb_FatCount]              ; ax = FatCount
        mul word [bpb_SectorsPerFAT]
        add ax, [bpb_ReservedSectors]       ; ax = RootDirectoryLBA
        push ax
        mov ax, 32                          ; ax = sizeof(DirectoryEntry)
        mul word [bpb_RootDirEntries]       ; ax = RootDirSize
        add ax, 511
        xor dx, dx
        div word [bpb_BytesPerSector]       ; ax = RootDirectorySectors
        mov cx, ax                          ; cl = RootDirectorySectors
        pop ax                              ; ax = RootDirectoryLBA
        mov bx, ax
        add bx, cx                          ; bx = DataSectionLBA
        mov word [g_DataSectionLBA], bx
        xor bx, bx
        mov es, bx
        mov bx, buffer                      ; es:bx = 0x0000:buffer
        mov dl, [ebr_DriveNumber]           ; dl = boot disk
        call ReadSectors
    mov si, buffer
    .FindFile:
        push si
        mov cx, 11                          ; compare up to 11 bytes
        mov di, g_File                      ; compare si to g_File
        repe cmpsb
        pop si
        je .FoundFile
        add si, 32
        jmp .FindFile
    .FoundFile:
        test si, si
        jz ReadSectors.fail
        add si, 26                          ; si = g_File.FirstClusterLow
        mov ax, [si]
        push ax
        STAGE2SEGMENT equ 0x0000
        STAGE2OFFSET equ 0x0500
    .ReadFAT:
        mov ax, [bpb_ReservedSectors]
        mov bx, 0
        mov es, bx
        mov bx, buffer
        mov cx, [bpb_SectorsPerFAT]
        mov dl, [ebr_DriveNumber]
        call ReadSectors
        pop ax
        mov bx, STAGE2SEGMENT
        mov es, bx
        mov bx, STAGE2OFFSET
    .ReadFile:
        cmp ax, 0xFF8
        jae .Finish
        push ax
        call Cluster2LBA
        mov cl, [bpb_SectorsPerCluster]
        mov dl, [ebr_DriveNumber]
        call ReadSectors
        mov ax, [bpb_BytesPerSector]
        xor ch, ch
        mul cx
        add bx, ax                      ; will wraparound if stage2.bin is more than 64kb
        pop ax
        mov cx, 3
        mul cx
        mov cx, 2
        xor dx, dx
        div cx
        add ax, buffer
        mov si, ax
        mov ax, [si]
        cmp dx, 0
        jne .odd
    .even:
        and ax, 0x0FFF
        jmp .ReadFile
    .odd:
        shr ax, 4
        jmp .ReadFile
    .Finish:
        mov ax, STAGE2SEGMENT
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov sp, 0
        mov bp, sp
        mov dl, [ebr_DriveNumber]
        jmp STAGE2SEGMENT:STAGE2OFFSET

times 510-($-$$) db 0           ; pad unused disk space with 0
db 0x55, 0xAA                   ; BIOS boot signature
buffer: