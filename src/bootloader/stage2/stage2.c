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
    ///*
    DISK disk;
    ClrScr();
    printf("COOLBOOT Stage2 v0.0.06 Booted from drive: 0%xh\n", BootDrive);
    printf("Enabling A20 line\n");
    EnableA20();
    printf("Initialising disk 0%xh\n", BootDrive);
    DiskInitialise(&disk, BootDrive);
    printf("Disk 0%xh: %u Tracks, %u Sides, %u Sectors Per Track\n", BootDrive, disk.Cylinders, disk.Heads, disk.SectorsPerCylinder);
    printf("Initialising FAT\n");
    FatInitialise(&disk, BootDrive);
    printf("Root address: 0x%x\n", g_CurrentDirectory);
    printf("Fat address: 0x%x\n", g_FAT);
    printf("Loading Kernel\n");
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, "system");
    if(!fd) {
        printf("System Directory not found\n");
        return;
    }
    DirectoryEntry* DirectoryBuffer = (DirectoryEntry*)malloc(g_FatData.BootSect.u_BootSector.RootDirEntries * sizeof(DirectoryEntry));
    ReadFile(&disk, BootDrive, fd, DirectoryBuffer);
    fd = FindFile(DirectoryBuffer, "kernel.bin");
    if(!fd) {
        printf("kernel.bin not found\n");
        return;
    }
    ReadFile(&disk, BootDrive, fd, KERNEL_START);
    int (*StartKernel)(uint8_t) = KERNEL_START;
    int ErrCode = StartKernel(BootDrive);
    ClrScr();
    CHAR_COLOUR = 0xF4;
    printf("FATAL: kernel program terminated with status %i\n", ErrCode);
    printf("See documentation for more information\n");
    return;
}