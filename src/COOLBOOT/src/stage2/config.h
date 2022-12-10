#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "memory.h"
#include "disk.h"
#include "FAT.h"
#include <stdint.h>
#include <stdbool.h>

char* g_COOLBOOTSYS = (char*)NULL;
char* g_COOLBOOTSYS_BAK = (char*)NULL;             // a very inefficient bug fix
uint32_t g_COOLBOOTSIZE = 0;

// assumes the FAT has already been initialised
bool InitialiseConfig(DISK* disk, uint8_t Drive) {
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, "coolboot.sys");
    if(!fd) {
        printf("Could not find configuration file \"/coolboot.sys\"\n");
        return false;
    }
    g_COOLBOOTSIZE = fd->Size + 1024;
    g_COOLBOOTSYS = (char*)malloc(g_COOLBOOTSIZE);
    if(!ReadFile(disk, Drive, fd, g_COOLBOOTSYS)) return false;
    g_COOLBOOTSYS_BAK = (char*)malloc(g_COOLBOOTSIZE);
    memcpy(g_COOLBOOTSYS_BAK, g_COOLBOOTSYS, g_COOLBOOTSIZE);
    return true;
}

char* GetOption(char* option) {
    char* OPTION = strstr(g_COOLBOOTSYS_BAK, option);
    while(*(OPTION - strlen(option) - 1) != ':' || *OPTION != '=') {
        OPTION = strstr(OPTION, option);
        if(!OPTION) {
        printf("Could not find option %s\n", option);
        return (char*)NULL;
    }
    }
    OPTION++;
    char* endl = strchr(OPTION, ';');
    if(!endl) {
        printf("Syntax error: expected ';' at end of option %s\n", option);
        return (char*)NULL;
    }
    *endl = 0;
    return OPTION;
}

#endif