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
    u8 display[64 * 32];
    u8 keyboard[16];
    u16 opcode;
    
} Chip8;

