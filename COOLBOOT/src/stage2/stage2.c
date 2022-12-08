// stage2.c 27/11/2022
// written by Gabriel Jickells

#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "A20.h"
#include "disk.h"
#include "FAT.h"
#include "config.h"

#define KERNEL_START (void*)0x20000             // chosen arbitrarily but it works
#define CONFIG_SIGN "CB23110512v0.0.09"
#define KERNEL_CODE_SUCCESS 0l

void main(const uint8_t BootDrive)
{
    DISK disk;
    ClrScr();
    printf("COOLBOOT Stage2 v0.0.12 Booted from drive: 0%xh\n", BootDrive);
    printf("Enabling A20 line\n");
    EnableA20();
    printf("Initialising disk 0%xh\n", BootDrive);
    DiskInitialise(&disk, BootDrive);
    printf("Disk 0%xh: %u Tracks, %u Sides, %u Sectors Per Track\n", BootDrive, disk.Cylinders, disk.Heads, disk.SectorsPerCylinder);
    printf("Initialising FAT\n");
    FatInitialise(&disk, BootDrive);
    printf("Root address: 0x%x\n", g_CurrentDirectory);
    printf("Fat address: 0x%x\n", g_FAT);
    InitialiseConfig(&disk, BootDrive);
    char* SIGNATURE = GetOption("SIGNATURE");
    if(!SIGNATURE) return;
    if(!memcmp(SIGNATURE, CONFIG_SIGN, strlen(SIGNATURE))) {
        printf("coolboot.sys signature is not valid\n");
        printf("see documentation for more information\n");
        return;
    }
    memcpy(g_COOLBOOTSYS_BAK, g_COOLBOOTSYS, g_COOLBOOTSIZE);   // reset the GetOption buffer
    char* KERNEL_FILE = GetOption("KERNEL_FILE");
    if(!KERNEL_FILE) return;
    char* KERNEL_FILE_NEXT = KERNEL_FILE;
    char* KERNEL_FILE_FINAL = KERNEL_FILE;
    char tmpf;
    for(;;) {
        KERNEL_FILE_NEXT = strchr(KERNEL_FILE_FINAL, '/');
        if(!KERNEL_FILE_NEXT) {
            tmpf = *KERNEL_FILE_FINAL;
            *KERNEL_FILE_FINAL = 0;
            break;
        }
        KERNEL_FILE_NEXT++;
        KERNEL_FILE_FINAL = KERNEL_FILE_NEXT;
    }
    if(!OpenDirectory(&disk, BootDrive, KERNEL_FILE)) {
        printf("Could not find kernel directory, please update coolboot.sys\n");
        return;
    }
    *KERNEL_FILE_FINAL = tmpf;
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, KERNEL_FILE_FINAL);
    if(!fd) {
        printf("Could not find kernel file\n");
        return;
    }
    ReadFile(&disk, BootDrive, fd, KERNEL_START);
    int (*StartKernel)(uint8_t) = KERNEL_START;
    int ErrCode = StartKernel(BootDrive);
    if(ErrCode == KERNEL_CODE_SUCCESS) return;
    ClrScr();
    CHAR_COLOUR = 0xF4;
    printf("COOLBOOT stage2 v0.0.11 post-kernel environment\n");
    printf("FATAL: kernel program terminated with status 0x%x\n", ErrCode);
    printf("See documentation for more information\n");
    return;
}