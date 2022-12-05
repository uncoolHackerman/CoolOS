# Changelogs (dates in DD/MM/YYYY format)

## stage2.bin v0.0.09 (05/12/2022)

- options must now start with a ':' prefix

## stage2.bin v0.0.08 (04/12/2022)

- coolboot.sys must now have a valid signature to make sure there aren't any formatting issues when the syntax inevitably changes, valid signatures for each version will be placed in the docs directory
- added some functions to parse coolboot.sys

## stage2.bin v0.0.07 (04/12/2022)

- changed hardcoded kernel.bin path to one that can be specified in a new file named "/coolboot.sys"
- added a function to open a directory path

## kernel.bin v0.0.01 (04/12/2022)

- added printf support for pointers
- added GDT definitions, although they are yet to be used in the kernel

## stage2.bin v0.0.06 (04/12/2022)

- changed the heap to start from the end of the program rather than at 0x7E00
- added string functions
  - char*strchr(char*str, char chr)
  - uint32_t strlen(char* str)
  - char ToUpper(char c)
- allowed FindFile() to parse 8.3 file names
- stage2.bin will now load the kernel file "/system/kernel.bin"
- changed CHAR_COLOUR to a variable rather than a preprocessor constant

## stage2.bin v0.0.05 (03/12/2022)

- added FAT functions
  - bool ReadBootRecord(DISK* disk, uint8_t Drive)
  - bool ReadRootDirectory(DISK*disk, uint8_t Drive, void* BufferOut)
  - DirectoryEntry*FindFile(DirectoryEntry* Dir, char* Name)
  - bool ReadFile(DISK*disk, uint8_t Drive, DirectoryEntry* file, void* BufferOut);
  - uint32_t Cluster2LBA(uint16_t Cluster);
  - bool ReadFat(DISK* disk, uint8_t Drive);
- added FAT structures
  - BootSector
  - DirectoryEntry
  - FAT_Data
- added more disk functions
  - bool DiskInitialise(DISK* disk, uint8_t drive)
- changed the printf state machine to start from 0 instead of 1
- added memory management functions
  - bool memcmp(void*ptr1, void* ptr2, uint32_t count)
  - void memcpy(void*dst, void* src, uint32_t count)
  - void memset(void* dst, uint8_t val, uint32_t count)
  - void* malloc(uint32_t size)
  - void free(void** FreeBird)
- added printf support for octal numbers
- removed the UpdateCursor function
- added support for screen scrolling

## stage2.bin v0.0.04 (02/12/2022)

- added disk functions:
  - bool BIOS_ReadSectors(uint8_t Drive, uint16_t Cylinder, uint16_t Head, uint16_t Sector, uint8_t Count, void* BufferOut)
  - bool BIOS_GetDriveParameters(uint8_t Drive, uint8_t*DriveType, uint16_t* Cylinders, uint16_t*Heads, uint16_t* Sectors)
  - bool BIOS_ResetDisk(uint8_t Drive)
  - bool ReadSectors(DISK*disk, uint8_t Drive, uint32_t lba, uint8_t Count, void* BufferOut)
  - void LBA2CHS(DISK*disk, uint32_t lba, uint16_t* Cylinder, uint16_t*Head, uint16_t* Sector)
- added DISK structure to hold Drive Parameters
