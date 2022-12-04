// stdio.h 27/11/2022
// written by Gabriel Jickells

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "memory.h"

/*
To-Do:
- give printf support for format specifiers longer than 1 byte
*/

#define CHAR_BUFFER (char*)0xb8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

uint32_t CursorX = 0;
uint32_t CursorY = 0;
uint8_t CHAR_COLOUR = 0x0F;

void putc(char c);
void ClrScr(void);
void puts(char* s);
void putnum(unsigned long num, int base, bool sign);
void printf(const char* fmt, ...);
void ScrollScreen(unsigned int lines);

void putc(char c)
{
    uint32_t Position = (uint32_t)CHAR_BUFFER + ((CursorY * SCREEN_WIDTH + CursorX) * 2);
    switch(c)
    {
        case '\n':
            CursorY++;
            CursorX = 0;
            break;
        case '\r':
            CursorX = 0;
            break;
        default:
            *(char*)Position = c;
            CursorX++;
            break;
    }
    Position++;
    *(char*)Position = CHAR_COLOUR;
    if(CursorX >= SCREEN_WIDTH) {
        CursorY++;
        CursorX = 0;
    }
    if(CursorY >= SCREEN_HEIGHT)
        ScrollScreen(1);
    return;
}

void ClrScr(void)
{
    memset(CHAR_BUFFER, 0, SCREEN_HEIGHT * SCREEN_WIDTH * 2);
    CursorX = 0;
    CursorY = 0;
    return;
}

void puts(char* s)
{
    while(*s)
    {
        putc(*s);
        s++;
    }
    return;
}

char g_Hex[] = "0123456789ABCDEF";

void putnum(unsigned long num, int base, bool sign)
{
    char Buffer[32] = {0};
    int i = 0;
    signed long oldnum = (signed long)num;
    if(sign && (signed long)num < 0) num = -num;
    uint32_t rem;
    do
    {
        rem = num % base;
        num /= base;
        Buffer[i] = g_Hex[rem];
        i++;
    } while(num);
    if(sign && oldnum < 0) Buffer[i] = '-';
    for(int j = 31; j >= 0; j--)
        if(Buffer[j])putc(Buffer[j]);
    return;
}

enum PrintfStates
{
    PRINTF_STATE_NORMAL = 0,
    PRINTF_STATE_TYPE
};

// very basic, needs to be made better to support format specifiers more than 1 byte (%hhx, %lu, etc...)
void printf(const char* fmt, ...)
{
    int state = PRINTF_STATE_NORMAL;
    va_list args;
    va_start(args, fmt);
    while(*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_TYPE:
                switch (*fmt)
                {
                    case 's':
                        puts(va_arg(args, char*));
                        break;
                    case '%':
                        putc('%');
                        break;
                    case 'c':
                        putc((char)va_arg(args, int));
                        break;
                    case 'u':
                        putnum(va_arg(args, unsigned int), 10, 0);
                        break;
                    case 'x':
                        putnum(va_arg(args, unsigned int), 16, 0);
                        break;
                    case 'i':
                        putnum(va_arg(args, int), 10, 1);
                        break;
                    case 'o':
                        putnum(va_arg(args, unsigned int), 8, 0);
                        break;
                    default:
                        break;
                }
                state = PRINTF_STATE_NORMAL;
                break;
            default:
                switch(*fmt)
                {
                    case '%':
                        state = PRINTF_STATE_TYPE;
                        break;
                    default:
                        putc(*fmt);
                        break;
                }
        }
        fmt++;
    }
    va_end(args);
    return;
}

void ScrollScreen(unsigned int lines) {
    CursorY -= lines;
    while(lines--) {
        for(uint32_t i = 0; i < (SCREEN_HEIGHT - 1); i++)
            memcpy(CHAR_BUFFER + (i * SCREEN_WIDTH * 2), CHAR_BUFFER + (i * SCREEN_WIDTH * 2 + SCREEN_WIDTH * 2), SCREEN_WIDTH * 2);
        memset(CHAR_BUFFER + ((SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2), 0, SCREEN_WIDTH * 2);
    }
    return;
}

#endif