// memory.h 03/12/2022
// written by Gabriel Jickells

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdbool.h>
#include <stdint.h>

/*
To-Do:
- create malloc and free functions
*/

bool memcmp(void* ptr1, void* ptr2, uint32_t count);
void memcpy(void* dst, void* src, uint32_t count);
void memset(void* dst, uint8_t val, uint32_t count);
void* malloc(uint32_t size);

// returns 1 if they are equal, returns 0 when they are different
bool memcmp(void* ptr1, void* ptr2, uint32_t count)
{
    uint8_t* charPtr1 = (uint8_t*)ptr1;
    uint8_t* charPtr2 = (uint8_t*)ptr2;
    for(uint32_t i = 0; i < count; i++)
    {
        if(*charPtr1 != *charPtr2) return false;
        charPtr1++;
        charPtr2++;
    }
    return true;
}

void memcpy(void* dst, void* src, uint32_t count) {
    uint8_t* charDst = (uint8_t*)dst;
    uint8_t* charSrc = (uint8_t*)src;
    for(uint32_t i = 0; i < count; i++) {
        *charDst = *charSrc;
        charDst++;
        charSrc++;
    }
}

void memset(void* dst, uint8_t val, uint32_t count) {
    uint8_t* charDst = (uint8_t*)dst;
    for(uint32_t i = 0; i < count; i++)
        charDst[i] = val;
    return;
}

static uint32_t MallocTable[255] = {0};
uint32_t MallocTableSize = 0;
uint8_t MallocIndex = 0;
#define NULL ((void *)0)

extern void* g_HeapStart;

// not a very good way of doing it
void* malloc(uint32_t size) {
    memset(g_HeapStart + MallocTableSize, 0, size);
    MallocTable[MallocIndex] = size;
    MallocTableSize += size;
    MallocIndex++;
    return g_HeapStart + MallocTableSize - size;
}

// takes pointer to last malloced pointer (e.g. &pointer)
void free(void** FreeBird) {
    MallocIndex--;
    MallocTableSize -= MallocTable[MallocIndex];
    MallocTable[MallocIndex] = 0;
    *FreeBird = NULL;
    return;
}

#endif