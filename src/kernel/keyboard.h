// keyboard.h 10/12/2022
// a keyboard driver for CoolOS v0.0.13
// Written by Gabriel Jickells

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "io.h"
#include <stdbool.h>
#include "string.h"
#include <stdint.h>

#define KBD_PORT 0x60

char g_KbdMap[] = {
    0, 1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '#',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'R', '*', 
    0, ' ', 'A', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'N', 'S', 
    '7', '8', '9', '-',
    '4', '5', '6', '+',
    '1', '2', '3', 0, 0, 0, 0, 
    '\\', 0, 0 //0x58
};

char g_KbdMap_Shift[] = {
    0, 1, '!', '\"', 0x9c, '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x08,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@', 0xAA, 0, '~',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'N', 'S', 
    '7', '8', '9', '-',
    '4', '5', '6', '+',
    '1', '2', '3', 0, 0, 0, 0, 
    '|', 0, 0 //0x58
};

bool g_Shift = false;
bool g_Caps = false;

char getc() {
    char c = inb(KBD_PORT);
    if((uint32_t)c % 256 == 0xAA || (uint32_t)c % 256 == 0xB6) {g_Shift = false; return 0;}
    if((uint32_t)c % 256 == 0x2A || (uint32_t)c % 256 == 0x36) {g_Shift = true; return 0;}
    if((uint32_t)c % 256 == 0x3A) {g_Caps = !g_Caps; return 0;}
    if(c & 0x80 || !c) return 0;
    if(g_Shift && !g_Caps) return g_KbdMap_Shift[c];
    if(g_Caps && !g_Shift) return ToUpper(g_KbdMap[c]);
    if(g_Caps && g_Shift) return ToLower(g_KbdMap_Shift[c]);
    return g_KbdMap[c];
}

#endif