/*
MIT License

Copyright (c) 2022 Lu-Die-Milchkuh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <stdint.h>
typedef uint8_t u8;   // 8-Bit aka 1 Byte
typedef uint16_t u16; // 16-Bit aka 2 Bytes,1 Word

typedef union RAM
{   
    struct
    {
        u8 fonts[512];      // Fonts, some programs expect them to be stored at 0-0x200
        u8 rom[(4096-512)]; // Array to store ROM Content
    };
    
    u8 ram[4096];       // System RAM
    
} RAM;

typedef struct Chip8
{
    RAM memory;     // Total System Memory
    u8 V[16];       // 16 Special Purpose Register
    u16 Stack[16];  // Stack
    u8 SP;          // Stack Pointer
    u8 ST;          // Sound Timer
    u8 DT;          // Delay Timer
    u16 PC;         // Programm Counter
    u16 Index;      // Index Register
    u8 display[64][32]; // 2048 KB of VRAM
    u8 keyboard[16];
    u16 opcode;
    
} Chip8;

