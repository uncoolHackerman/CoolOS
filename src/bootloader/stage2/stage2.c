// stage2.c 27/11/2022
// written by Gabriel Jickells

#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "A20.h"
#include "disk.h"
#include "FAT.h"

#define KERNEL_START (void*)0x20000             // chosen arbitrarily but it works

void main(const uint8_t BootDrive)
{
    DISK disk;
    ClrScr();
    printf("COOLBOOT Stage2 v0.0.07 Booted from drive: 0%xh\n", BootDrive);
    printf("Enabling A20 line\n");
    EnableA20();
    printf("Initialising disk 0%xh\n", BootDrive);
    DiskInitialise(&disk, BootDrive);
    printf("Disk 0%xh: %u Tracks, %u Sides, %u Sectors Per Track\n", BootDrive, disk.Cylinders, disk.Heads, disk.SectorsPerCylinder);
    printf("Initialising FAT\n");
    FatInitialise(&disk, BootDrive);
    printf("Root address: 0x%x\n", g_CurrentDirectory);
    printf("Fat address: 0x%x\n", g_FAT);
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, "coolboot.sys");
    if(!fd) {
        printf("Could not find configuration file \"/coolboot.sys\"");
        return;
    }
    char* Buffer = malloc(fd->Size + 512);
    ReadFile(&disk, BootDrive, fd, Buffer);
    char* KERNEL_FILE = strstr(Buffer, "KERNEL_FILE=");
    char* endl = strchr(KERNEL_FILE, ';');
    if(!endl) {
        printf("Syntax error\n");
        return;
    }
    *endl = 0;
    if(!KERNEL_FILE) {
        printf("There is no KERNEL_FILE option in the configuration file\n");
        return;
    }
    free((void**)&Buffer);
    char* KERNEL_FILE_NEXT = KERNEL_FILE;
    char* KERNEL_FILE_FINAL = KERNEL_FILE;
    for(;;) {
        KERNEL_FILE_NEXT = strchr(KERNEL_FILE_FINAL, '/');
        if(!KERNEL_FILE_NEXT) {
            *(KERNEL_FILE_FINAL - 1) = 0;
            break;
        }
        KERNEL_FILE_NEXT++;
        KERNEL_FILE_FINAL = KERNEL_FILE_NEXT;
    }
    if(!OpenDirectory(&disk, BootDrive, KERNEL_FILE)) {
        printf("Could not find kernel directory, please update coolboot.sys\n");
        return;
    }
    fd = FindFile(g_CurrentDirectory, KERNEL_FILE_FINAL);
    if(!fd) {
        printf("Could not find kernel file\n");
        return;
    }
    ReadFile(&disk, BootDrive, fd, KERNEL_START);
    int (*StartKernel)(uint8_t) = KERNEL_START;
    int ErrCode = StartKernel(BootDrive);
    ClrScr();
    CHAR_COLOUR = 0xF4;
    printf("FATAL: kernel program terminated with status 0x%x\n", ErrCode);
    printf("See documentation for more information\n");
    return;
}