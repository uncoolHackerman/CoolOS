// FatFind.c 25/11/2022
// Written by Gabriel Jickells
// this tool will find a file in the root directory of a fat12 formatted disk

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

FILE* g_disk;

typedef struct
{
    char Identifier[11];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t RootDirEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t HeadCount;
    uint32_t HiddenSectors;
    uint32_t LargeSector;
    uint8_t DriveNumber;
    uint8_t WinNTFlags;
    uint8_t Signature;
    char SerialNumber[4];
    char VolumeID[11];
    char SystemID[8];
} __attribute__((packed)) BootSector;

typedef struct
{
    char Name[11];
    uint8_t Attributes;
    uint8_t Reserved;
    uint8_t CreationTimeTenths;
    uint16_t CreationTime;
    uint16_t CreationDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModificationTime;
    uint16_t ModificationDate;
    uint16_t FirstClusterLow;
    uint32_t Size;                      // measured in bytes
} __attribute__((packed)) DirectoryEntry;


BootSector g_BootSector;
DirectoryEntry* g_RootDirectory = NULL;         // needs to be allocated using malloc
uint32_t g_DataSectionLBA;
uint8_t* g_FAT = NULL;

int ReadBootSector()
{
    return fread(&g_BootSector, sizeof(g_BootSector), 1, g_disk);
}

int ReadSectors(uint32_t lba, uint32_t count, void* BufferOut)
{
    if(fseek(g_disk, lba * g_BootSector.BytesPerSector, SEEK_SET) != 0) return 0;
    if(!(fread(BufferOut, g_BootSector.BytesPerSector, count, g_disk) > 0)) return 0;
    return 1;
}

int ReadRootDirectory()
{
    uint32_t RootDirectoryLBA = g_BootSector.ReservedSectors + (g_BootSector.FatCount * g_BootSector.SectorsPerFAT);
    uint32_t RootDirectorySize = g_BootSector.RootDirEntries * sizeof(DirectoryEntry);
    uint32_t RootDirectorySectors = RootDirectorySize / 512;
    if(RootDirectorySize % 512) RootDirectorySectors++;
    g_DataSectionLBA = RootDirectoryLBA + RootDirectorySectors;
    g_RootDirectory = (DirectoryEntry*)malloc(RootDirectorySectors * g_BootSector.BytesPerSector);
    return ReadSectors(RootDirectoryLBA, RootDirectorySectors, g_RootDirectory);
}

DirectoryEntry* FindFile(char* Name)
{
    for(int i = 0; i < g_BootSector.RootDirEntries; i++)
    {
        if(memcmp(Name, g_RootDirectory[i].Name, 11) == 0)
        return &g_RootDirectory[i];
    }
    return NULL;
}

int ReadFAT()
{
    g_FAT = (uint8_t*)malloc(g_BootSector.SectorsPerFAT * g_BootSector.BytesPerSector);
    return ReadSectors(g_BootSector.ReservedSectors, g_BootSector.SectorsPerFAT, g_FAT);
}

uint32_t Cluster2LBA(uint16_t cluster)
{
    return g_DataSectionLBA + (cluster - 2) * g_BootSector.SectorsPerCluster; 
}

int ReadFile(DirectoryEntry* file, void* BufferOut)
{
    uint16_t CurrentCluster = file->FirstClusterLow;
    uint32_t FatIndex;
    do
    {
        if(!ReadSectors(Cluster2LBA(CurrentCluster), g_BootSector.SectorsPerCluster, BufferOut)) return 0;
        BufferOut += g_BootSector.SectorsPerCluster * g_BootSector.BytesPerSector;
        FatIndex = CurrentCluster * 3 / 2;
        if(CurrentCluster & 1) CurrentCluster = *(uint16_t*)(g_FAT + FatIndex) >> 4;
        else CurrentCluster = *(uint16_t*)(g_FAT + FatIndex) & 0xFFF;
    } while (CurrentCluster < 0x0FF8);
    return 1;
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Syntax %s: <disk image> <file in fat format (\"FILE    TXT\" padded with spaces)>\n", argv[0]);
        return -1;
    }
    g_disk = fopen(argv[1], "rb");
    if(!g_disk)
    {
        printf("Could not open disk image \"%s\"\n", argv[1]);
        return -2;
    }
    if(!ReadBootSector())
    {
        printf("Could not read boot sector\n");
        return -3;
    }
    if(!ReadRootDirectory())
    {
        printf("Could not read root directory\n");
        return -4;
    }
    DirectoryEntry* FileEntry = FindFile(argv[2]);
    if(!FileEntry)
    {
        printf("Could not find file %s\n", argv[2]);
        return -5;
    }
    if(!ReadFAT())
    {
        printf("Could not read FAT\n");
        return -6;
    }
    printf("File: %.11s\nSize: %x\n\n", FileEntry->Name, FileEntry->Size);
    char* Buffer = (char*)malloc(FileEntry->Size + g_BootSector.BytesPerSector);
    ReadFile(FileEntry, Buffer);
    printf("%s\n", Buffer);
    return 0;
}