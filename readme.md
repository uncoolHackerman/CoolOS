<<<<<<< HEAD
# Changelogs

## stage2.bin v0.0.04 (02/12/2022)
=======
<h1>Changelogs</h1>

<h2>stage2.bin v0.0.04 (02/12/2022)</h2>
>>>>>>> 7e5a101d07ba9b08989c18e1b82534ecff72d96d

- added disk functions:
  - bool BIOS_ReadSectors(uint8_t Drive, uint16_t Cylinder, uint16_t Head, uint16_t Sector, uint8_t Count, void* BufferOut)
  - bool BIOS_GetDriveParameters(uint8_t Drive, uint8_t*DriveType, uint16_t* Cylinders, uint16_t*Heads, uint16_t* Sectors)
  - bool BIOS_ResetDisk(uint8_t Drive)
  - bool ReadSectors(DISK*disk, uint8_t Drive, uint32_t lba, uint8_t Count, void* BufferOut)
  - void LBA2CHS(DISK*disk, uint32_t lba, uint16_t* Cylinder, uint16_t*Head, uint16_t* Sector)
- added DISK structure to hold Drive Parameters

<<<<<<< HEAD
## stage2.bin v0.0.05 (03/12/2022)
=======
<h2>stage2.bin v0.0.05 (03/12/2022)</h2>
>>>>>>> 7e5a101d07ba9b08989c18e1b82534ecff72d96d

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