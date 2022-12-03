// stage2.c 27/11/2022
// written by Gabriel Jickells

#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "A20.h"
#include "disk.h"
#include "FAT.h"

/*
To-do:
- write some FAT reading functions (readfile, findfile, readRoot, etc...)
- write some memory management functions
*/

void main(const uint8_t BootDrive)
{
    DISK disk;
    ClrScr();
    printf("COOLBOOT Stage2 v0.0.05 Booted from drive: 0%xh\n", BootDrive);
    printf("Enabling A20 line\n");
    EnableA20();
    printf("Initialising disk 0%xh\n", BootDrive);
    if(!DiskInitialise(&disk, BootDrive)) return;
    printf("Disk 0%xh: %u Tracks, %u Sides, %u Sectors Per Track\n", BootDrive, disk.Cylinders, disk.Heads, disk.SectorsPerCylinder);
    printf("Initialising FAT\n");
    FatInitialise(&disk, BootDrive);
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, "SRC        ");
    if(!fd) {
        printf("Could not find directory\n");
        return;
    }
    DirectoryEntry* DirectoryBuffer = (DirectoryEntry*)malloc(g_FatData.BootSect.u_BootSector.RootDirEntries * sizeof(DirectoryEntry));
    ReadFile(&disk, BootDrive, fd, DirectoryBuffer);
    printf("contents of %s:\n", fd->Name);
    for(int i = 0; i < g_FatData.BootSect.u_BootSector.RootDirEntries; i++)
    {
        if(DirectoryBuffer[i].Name[0] == '\0') break;
        puts("  ");
        puts(DirectoryBuffer[i].Attributes & 0x10 ?  "<DIR> ": "<FILE> ");
        for(int j = 0; j < 11; j++)
        {
            if(j == 8) putc(DirectoryBuffer[i].Attributes & 0x10 ? 0 : '.');
            if(DirectoryBuffer[i].Name[j] != ' ') putc(DirectoryBuffer[i].Name[j]);
        }
        putc('\n');
    }
    return;
}