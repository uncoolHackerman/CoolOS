/*
boot.s 11/12/2022
COOLBOOT bootstrap re-written in at&t syntax because it's a little easier to read
Written by Gabriel Jickells
*/

.code16
.globl _start

jmp _start /* skip the FAT data */
nop
/* BIOS Parameter Block */
BPB_Identifier: .ascii "COOLBOOT"   /* OEM Identifier, 8 bytes */
BPB_BytesPerSector: .word 512
BPB_SectorsPerCluster: .byte 2
BPB_ReservedSectors: .word 1        /* Reserved Sectors (also the LBA of the FATs) */
BPB_FATs: .byte 2                   /* Total FATs on the disk */
BPB_RootDirEntries: .word 224       /* Total Root Directory Entries (obtained from mkfs.fat) */
BPB_TotalSectors: .word 2880
BPB_MediaDescriptor: .byte 0xF0     /* Media Descriptor Type */
BPB_SectorsPerFat: .word 9
BPB_SectorsPerTrack: .word 18
BPB_HeadCount: .word 2
BPB_HiddenSectors: .long 0          /* LBA of the start of the partition */
BPB_LargeSectors: .long 0
/* Extended Boot Record */
EBR_DriveNumber: .byte 0x00         /* BIOS drive number */
EBR_Reserved: .byte 0
EBR_Signature: .byte 0x28           /* must be 0x28 or 0x29 */
EBR_SerialNumber: .ascii "GJv0"
EBR_VolumeID: .ascii "GJ.COOLBOOT"
EBR_SystemID: .ascii "FAT12   "

_start:
    /* set up stack and segment registers */
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov $0x7c00, %sp
    mov %sp, %bp
    ljmp $0, $main                  /* Reset CS to 0 */


/*
-ARGUMENTS-
si = address of string
*/
puts:
    pusha                           /* push ax, cx, dx, bx, sp, bp, si, and di in that order */
    movb $0x0E, %ah                 /* teletype output */
    xorb %bh, %bh                   /* page 0 */
    .loop:
        lodsb                       /* loads [ds:si] into al and increments si */
        testb %al, %al              /* checks for end of string */
        jz .end
        int $0x10                   /* BIOS video services */
        jmp .loop
    .end:
        popa
        ret

/*
-ARGUMENTS-
ax = LBA address
-OUTPUT-
ch = low 8 bits of cylinder number
cl = sector number
dh = head number
*/
LBA2CHS:
    pushw %ax
    pushw %dx
    xorw %dx, %dx
    divw (BPB_SectorsPerTrack)      /* ax = LBA / SectorsPerTrack */
    movb %dl, %cl                   /* cl = LBA % SectorsPerTrack */
    incw %cx                        /* cl = Sector Number */
    xorw %dx, %dx
    divw (BPB_HeadCount)            /* ax = (LBA / SectorsPerTrack) / Heads */
    movb %al, %ch                   /* ch = Cylinder Low */
    shlb $6, %ah
    orb %ah, %cl                    /* cl = (Cylinder High << 6) | Sector */
    movb %dl, %al
    popw %dx
    movb %al, %dh                   /* dh = Head Number */
    popw %ax
    ret

/*
-ARGUMENTS-
ax = LBA address
es:bx = output buffer
cl = count
dl = drive
*/
ReadSectors:
    pusha
    pushw %cx
    call LBA2CHS
    popw %ax
    movw $3, %di
    .retry:
        testw %di, %di
        jz reboot
        decw %di
        movb $0x02, %ah
        stc                         /* error handling B) */
        int $0x13
        jc .retry
        jmp .ReadSectorsEnd
    reboot:
        ljmp $0xF000, $0xFFF0       /* start of the BIOS according to the lord and saviour - bochs */
    .ReadSectorsEnd:
        popa
        ret

/*
-ARGUMENTS-
ax = cluster
-OUTPUT-
ax = LBA
*/
Cluster2LBA:
    subw $2, %ax
    mulb (BPB_SectorsPerCluster)
    addw (g_DataSectionLBA), %ax
    ret

MSG_Loading: .asciz "COOLBOOT v0.0.04\n\r"
g_DataSectionLBA: .word 0
g_File: .ascii "STAGE2  BIN"
/*FILE: .ascii "123456789AB" */

main:
    movw $MSG_Loading, %si
    call puts
    movb %dl, (EBR_DriveNumber)                 /* get drive parameters */
    xorw %di, %di
    movw %di, %es
    movb (EBR_DriveNumber), %dl
    movb $0x08, %ah
    stc
    int $0x13
    jc reboot
    xorb %ch, %ch                               /* store the drive paramters in the BPB */
    andb $0x3F, %cl
    movw %cx, (BPB_SectorsPerTrack)
    movb %dh, %dl
    xorb %dh, %dh
    incw %dx
    movw %dx, (BPB_HeadCount)
    xorw %di, %di
    movw %di, %es
    .ReadRootDirectory:
        xorw %ax, %ax
        movb (BPB_FATs), %al
        mulw (BPB_SectorsPerFat)
        addw (BPB_ReservedSectors), %ax         /* ax = RootDirectoryLBA */
        pushw %ax
        movw $32, %ax                           /* ax = sizeof(DirectoryEntry) */
        mulw (BPB_RootDirEntries)               /* ax = RootDirectorySize */
        addw $511, %ax
        xorw %dx, %dx
        divw (BPB_BytesPerSector)               /* ax = RootDirectorySectors */
        movw %ax, %cx                           /* cx (cl) = RootDirectroySectors */
        popw %ax                                /* ax = RootDirectoryLBA */
        movw %ax, %bx
        addw %cx, %bx                           /* bx = DataSectionLBA */
        movw %bx, (g_DataSectionLBA)
        xorw %bx, %bx
        movw %bx, %es
        movw $buffer, %bx
        movb (EBR_DriveNumber), %dl
        call ReadSectors
    movw $buffer, %si
    .FindFile:
        pushw %si
        movw $11, %cx                           /* compare up to 11 bytes */
        movw $g_File, %di                       /* compare si to g_File */
        repe cmpsb                              /* while(*(ds:si) == *(es:di)) { si++; di++; } */
        popw %si
        je .FoundFile
        addw $32, %si
        jmp .FindFile
    .FoundFile:
        testw %si, %si
        jz reboot
        addw $26, %si                            /* si = g_File.FirstClusterLow */
        movw (%si), %ax
        pushw %ax
    .ReadFAT:
        movw (BPB_ReservedSectors), %ax
        movw $0, %bx
        movw %bx, %es
        movw $buffer, %bx
        movw (BPB_SectorsPerFat), %cx
        movb (EBR_DriveNumber), %dl
        call ReadSectors
        popw %ax
        movw $0, %bx
        movw %bx, %es
        movw $0x500, %bx
    .ReadFile:
        cmpw $0xFF8, %ax
        jae .Finish
        pushw %ax
        call Cluster2LBA
        movb (BPB_SectorsPerCluster), %cl
        movb (EBR_DriveNumber), %dl
        call ReadSectors
        movw (BPB_BytesPerSector), %ax
        xorb %ch, %ch
        mulw %cx
        addw %ax, %bx                       /* will wraparound if stage2.bin is more than 64kb */
        popw %ax
        movw $3, %cx
        mulw %cx
        movw $2, %cx
        xorw %dx, %dx
        divw %cx
        addw $buffer, %ax
        movw %ax, %si
        movw (%si), %ax
        testw %dx, %dx
        jnz .odd
    .even:
        andw $0x0FFF, %ax
        jmp .ReadFile
    .odd:
        shrw $4, %ax
        jmp .ReadFile
    .Finish:
        movw $0, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss
        movw $0, %sp
        movw %sp, %bp
        movb (EBR_DriveNumber), %dl
        ljmp $0, $0x500

.org 510
.word 0xAA55                            /* BIOS boot signature */
buffer:
