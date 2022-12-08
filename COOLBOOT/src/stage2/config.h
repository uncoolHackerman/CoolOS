#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "memory.h"
#include "disk.h"
#include "FAT.h"
#include <stdint.h>
#include <stdbool.h>

char* g_COOLBOOTSYS = NULL;

// assumes the FAT has already been initialised
bool InitialiseConfig(DISK* disk, uint8_t Drive) {
    DirectoryEntry* fd = FindFile(g_CurrentDirectory, "coolboot.sys");
    if(!fd) {
        printf("Could not find configuration file \"/coolboot.sys\"\n");
        return false;
    }
    g_COOLBOOTSYS = (char*)malloc(fd->Size + 512);
    if(!ReadFile(disk, Drive, fd, g_COOLBOOTSYS)) return false;
    return true;
}

char* GetOption(char* option) {
    char* OPTION = strstr(g_COOLBOOTSYS, option);
    while(*(OPTION - strlen(option) - 1) != ':') {
        OPTION = strstr(OPTION, option);
        if(!OPTION || *OPTION != '=') {
        printf("Could not find option %s\n", option);
        return NULL;
    }
    }
    if(!OPTION || *OPTION != '=') {
        printf("Could not find option %s\n", option);
        return NULL;
    }
    OPTION++;
    char* endl = strchr(OPTION, ';');
    if(!endl) {
        printf("Syntax error: expected ';' at end of option %s\n", option);
        return false;
    }
    *endl = 0;
    return OPTION;
}

#endif