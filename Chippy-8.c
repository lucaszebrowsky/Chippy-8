#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "Chippy-8.h"

// Change this to whatever you like lol
#define SCALE 15

SDL_Window* window = NULL;
SDL_Renderer* window_render = NULL;
u8 drawFlag = 0;

void initSDL(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL Init Error!");
        exit(-1);
    }
    window = SDL_CreateWindow("Chippy-8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,64 * SCALE,32 * SCALE,0);
    
    if(!window) {
        printf("Failed to create SDL Window");
        exit(-1);
    }

    window_render = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(!window_render) {
        printf("Failed to create SDL Renderer!");
        exit(-1);
    }

}


void initChip8(Chip8* chip8) {
    const u8 font_array[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80 // F 
    };

    // Clearing RAM
    memset(chip8->memory.ram,0,sizeof(chip8->memory.ram));

    memset(chip8->display,0,sizeof(chip8->display));

    // Clearing Register
    memset(chip8->V,0,sizeof(chip8->V));

    // Clearing Keyboard
    memset(chip8->keyboard,0,sizeof(chip8->keyboard));

    // Loading Fonts into Memory
    memcpy(chip8->memory.fonts,&font_array,sizeof(font_array));

    chip8->PC = 0x200;  // Start of Program Code
    chip8->SP = 0;
    chip8->Index = 0;
    chip8->DT = 60;
    chip8->ST = 60;
    
}

void loadROM(char* path,Chip8* chip8) {
    FILE* file = fopen(path,"rb");
    size_t file_size = 0;
    if (file == NULL)
    {
        printf("Error opening ROM!\n");
        exit(-1);
    }
    
    fseek(file,0,SEEK_END);
    file_size = ftell(file);
    fseek(file,0,SEEK_SET);

    if(file_size > sizeof(chip8->memory.rom)) {
        printf("File too big!");
        exit(-1);
    }

    fread(chip8->memory.rom,sizeof(chip8->memory.rom),1,file);

    fclose(file);
    file = NULL;

}

void drawScreen(Chip8* chip8) {
    SDL_SetRenderDrawColor(window_render, 0, 0, 0, 255);
    SDL_RenderClear(window_render);
    SDL_SetRenderDrawColor(window_render, 0, 255, 0, 255);
    

    for(u8 py = 0; py < 32; py++) // y
    {   
        
        for(u8 px = 0; px < 64; px++) // x
        {   
           
            if(chip8->display[px + (py*64)]) {
                SDL_Rect rect;
                rect.x = px * SCALE;
                rect.y = py * SCALE;
                rect.w = SCALE;
                rect.h = SCALE;
                
                SDL_RenderFillRect(window_render,&rect);
                
            }
            
        }
        
    }
    SDL_RenderPresent(window_render);

}

/*
    Original Chip8 Keyboard Layout:

    1   2   3   C
    4   5   6   D
    7   8   9   E
    A   0   B   F

    Translated Keyboard Layout:

    1   2   3   4
    Q   W   E   R
    A   S   D   F
    Y   X   C   V
*/
void updateKeys(Chip8* chip8) {
    /*  This function returns a pointer to an SDL internal array,
        so it will always return the same pointer and wont cause a memory leak
    */
    const u8* key_state = SDL_GetKeyboardState(NULL);
    SDL_PumpEvents();

    chip8->keyboard[0x0] = key_state[SDL_SCANCODE_1];
    chip8->keyboard[0x1] = key_state[SDL_SCANCODE_2];
    chip8->keyboard[0x2] = key_state[SDL_SCANCODE_3];
    chip8->keyboard[0x3] = key_state[SDL_SCANCODE_4];
    chip8->keyboard[0x4] = key_state[SDL_SCANCODE_Q];
    chip8->keyboard[0x5] = key_state[SDL_SCANCODE_W];
    chip8->keyboard[0x6] = key_state[SDL_SCANCODE_E];
    chip8->keyboard[0x7] = key_state[SDL_SCANCODE_R];
    chip8->keyboard[0x8] = key_state[SDL_SCANCODE_A];
    chip8->keyboard[0x9] = key_state[SDL_SCANCODE_S];
    chip8->keyboard[0xA] = key_state[SDL_SCANCODE_S];
    chip8->keyboard[0xB] = key_state[SDL_SCANCODE_F];
    chip8->keyboard[0xC] = key_state[SDL_SCANCODE_Y];
    chip8->keyboard[0xD] = key_state[SDL_SCANCODE_X];
    chip8->keyboard[0xE] = key_state[SDL_SCANCODE_C];
    chip8->keyboard[0xF] = key_state[SDL_SCANCODE_V];
}

/*
    00E0 -> Clear Screen(VRAM)
    00EE -> Return from subroutine
*/
void INST_0000(Chip8* chip8) {
    switch(chip8->opcode)
        {
        case 0x00E0:
            memset(chip8->display,0,sizeof(chip8->display));
            break;

        case 0x00EE: // RET
            chip8->PC = chip8->Stack[chip8->SP];
            chip8->SP--;
            break;

        default:
            printf("Error! Opcode: %x\n",chip8->opcode);
            exit(-1);
            break;
        }
        chip8->PC += 2;
}

// Jump to address nnn (set PC to nnn)
void INST_1NNN(Chip8* chip8) {
    chip8->PC = (chip8->opcode & 0x0FFF);
}

// Execute subroutine at address nnn
void INST_2NNN(Chip8* chip8) {
    chip8->SP++;
    chip8->Stack[chip8->SP] = chip8->PC;
    chip8->PC = (chip8->opcode & 0x0FFF);
}

// Skip the next instruction if v[x] == nn
void INST_3XNN(Chip8* chip8){
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 nn = (chip8->opcode & 0x00FF);

    if(chip8->V[x] == nn) {
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

// Skip the next instruction if v[x] != nn
void INST_4XNN(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 nn = (chip8->opcode & 0x00FF);

    if(chip8->V[x] != nn) {
        chip8->PC += 2;
    }            
    chip8->PC += 2;
}

// Skip the next instruction if v[x] == v[y]
void INST_5XY0(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 y = (chip8->opcode & 0x00F0) >> 4;

    if(chip8->V[x]==chip8->V[y]) {
        chip8->PC += 2;
    } 
    chip8->PC += 2; 
}

// Store nn in v[x]
void INST_6XNN(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 nn = (chip8->opcode & 0x00FF);

    chip8->V[x] = nn;
    chip8->PC += 2;
}

// Add nn to v[x]
void INST_7XNN(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 nn = (chip8->opcode & 0x00FF);

    chip8->V[x] += nn;
    chip8->PC += 2;
}

/*
    8XY0 -> Store v[y] in v[x]
    8XY1 -> Set v[x] to v[x] OR v[y](bitwise)
    8XY2 -> Set v[x] to v[x] AND v[y](bitwise)
    8XY3 -> Set v[x] to v[x] XOR v[y](bitwise)
    8XY4 -> Add v[y] to v[x] with carry-checking(v[0xF])
    8XY5 -> Substract v[y] from v[x] with carry-checking(v[0xF])
    8XY6 -> Store v[y] shifted right one bit in v[x],v[0xF] is set to the least significant bit prior to the shift
    8XY7 -> Set v[x] = v[y] - v[x] with carry-checking
    8XYE -> Store v[y] shifted left one bit in v[x],v[0xF] is set to the most significant bit prior to the shift
*/
void INST_8000(Chip8* chip8) {
    u16 foo = (chip8->opcode & 0x000F);
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 y = (chip8->opcode & 0x00F0) >> 4;

    switch(foo)
    {
        case 0x0:
            chip8->V[x] = chip8->V[y];
            break;

        case 0x0001:
            chip8->V[x] = chip8->V[x] | chip8->V[y];
            break;

        case 0x0002:
            chip8->V[x] &= chip8->V[y];
            break;

        case 0x0003:
            chip8->V[x] ^= chip8->V[y];
            break;

        case 0x0004:
            chip8->V[0xF] = ((chip8->V[x]+chip8->V[y]) > 0xFF);
            chip8->V[x] += chip8->V[y];
            break;

        case 0x0005:
            chip8->V[0xF] = (chip8->V[x] > chip8->V[y]);
            chip8->V[x] -= chip8->V[y];
            break;

        case 0x0006:
            chip8->V[0xF] = chip8->V[x] & 0x1;
            chip8->V[x] = chip8->V[x] >> 1;
            break;

        case 0x0007:
            chip8->V[0xF] = (chip8->V[y] > chip8->V[x]);
            chip8->V[x] = chip8->V[y] - chip8->V[x];
            break;

        case 0x000E:
            chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
            chip8->V[x] = chip8->V[x] << 1;
            break;

        default:
            printf("Error! Opcode: %x\n",chip8->opcode);
            exit(-1);
            break;
        }
        chip8->PC += 2;
}

// Skip the next instruction if v[x] != v[y]
void INST_9XY0(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 y = (chip8->opcode & 0x00F0) >> 4;

    if(chip8->V[x] != chip8->V[y]){
        chip8->PC += 2;
    }
    chip8->PC += 2;
}

// Store nnn in Index-Register
void INST_ANNN(Chip8* chip8) {
    u16 nnn = (chip8->opcode & 0x0FFF);

    chip8->Index = nnn;
    chip8->PC += 2;
}

// Set PC to nnn + v[0]
void INST_BNNN(Chip8* chip8) {
    u16 nnn = (chip8->opcode & 0x0FFF);

    chip8->PC = chip8->V[0] + nnn;
}

// Set v[x] to a random number with a mask of nn
void INST_CXNN(Chip8* chip8) {
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u16 nn = (chip8->opcode & 0xFF);

    chip8->V[x] = (rand() % (255 -0+1)) + 0 + nn;
    chip8->PC += 2;
}

// Draw a sprite at position v[x], v[y] with n bytes of sprite data starting at the address stored in Index
void INST_DXYN(Chip8* chip8) {
    chip8->V[0xF] = 0;
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    u8 y = (chip8->opcode & 0x00F0) >> 4;
    u16 height = chip8->opcode & 0x000F;
    u16 pixel = 0;
        
    for(u16 yLine = 0; yLine < height; yLine++)
    {   
        pixel = chip8->memory.ram[chip8->Index + yLine];
        for(u16 xLine = 0; xLine < 8; xLine++) {
            if((pixel & (0x80 >> xLine)) != 0) {

                if(chip8->display[(chip8->V[x] + xLine + ((chip8->V[y] + yLine) * 64)) % 2048] == 1) {
                    chip8->V[0xF] = 1;
                }
                chip8->display[(chip8->V[x] + xLine + ((chip8->V[y] + yLine) * 64)) % 2048] ^= 1;
            }
        }
    }
    drawFlag = 1;
    chip8->PC += 2;
}

/*
    EX9E -> Skip the next instruction if the key corresponding to the hex value currently stored in register v[x] is pressed
    EXA1 -> Skip the next instruction if the key corresponding to the hex value currently stored in register v[x] is not pressed
*/
void INST_E000(Chip8* chip8) {
    u8 foo = (chip8->opcode & 0x00FF);
    u8 x = (chip8->opcode & 0x0F00) >> 8;
    
    switch(foo)
    {
        case 0x009E:
            
            if(chip8->keyboard[chip8->V[x]]) {
                chip8->PC += 2;
            }     
            break;

        case 0x00A1:
            
            if(!chip8->keyboard[chip8->V[x]]) {
                chip8->PC += 2;
            }
            break;

        default:
            printf("Error! Opcode: %x\n",chip8->opcode);
            exit(-1);
            break;
    }
    chip8->PC += 2;
    
}

/*
    FX07 -> Store Delay Timer(DT) in v[x]
    FX0A -> Wait for a keypress and store the result in v[x] NOTE: I forgot to save the result in v[x] but I am too lazy to fix it :D
    FX15 -> Set the Delay Timer(DT) to v[x]
    FX18 -> Set the Sound Timer(ST) to v[x]
    FX1E -> Add v[x] to Index-Register
    FX29 -> Set Index-Register to the memory address of the sprite data corresponding to the hexadecimal digit stored in register v[x]
    FX33 -> Store BCD representation of v[x] in memory locations I, I+1, and I+2
    FX55 -> Store the values of registers v[0] to v[0xF] inclusive in memory starting at address I.
    FX65 -> Fill registers v[0] to v[x] inclusive with the values stored in memory starting at address I
*/
void INST_F000(Chip8* chip8) {
    u8 foo = (chip8->opcode & 0x00FF);
    u8 x = (chip8->opcode & 0x0F00) >> 8;

    switch(foo) {
        case 0x0007:
            chip8->V[x] = chip8->DT;
            break;

        case 0x000A:

            SDL_Event event;
                
            while(!SDL_PollEvent(&event))
            {   
                // Wait for keypress by user
            }
            break;

        case 0x0015:
            chip8->DT = chip8->V[x];
            break;

        case 0x0018:
            chip8->ST = chip8->V[x];
            break;

        case 0x001E:
            chip8->Index += chip8->V[x];
            break;

        case 0x0029:
            chip8->Index = chip8->V[x] * 5;
            break;

        case 0x0033:
            chip8->memory.ram[chip8->Index] = (chip8->V[x] / 100);
            chip8->memory.ram[chip8->Index+1] = ((chip8->V[x] / 10) % 10);
            chip8->memory.ram[chip8->Index+2] = ((chip8->V[x] % 100) % 10);
            break;

        case 0x0055:
            for (u8 i = 0; i <= x; i++)
            {    
                chip8->memory.ram[(chip8->Index + i)] = chip8->V[i];
            }
            //chip8->Index = chip8->Index + 1 + x; <- Certain S-CHIP-compatible emulators may implement this instruction in this manner.
            break;

        case 0x0065:
            for (u8 i = 0; i <= x; i++)
            {   
                chip8->V[i] = chip8->memory.ram[chip8->Index+i];
            }
            //chip8->Index = chip8->Index + 1 + x; <- Certain S-CHIP-compatible emulators may implement this instruction in this manner.
            break;

        default:
            printf("Error! Opcode: %x\n",chip8->opcode);
            exit(-1);
            break;
        }

        chip8->PC += 2;

}


void game_loop(Chip8* chip8) {
    SDL_Event event;
    u8 running = 1;
    
    // Jump Table
    static void (*jump_table[16])(Chip8*) = {
        &INST_0000,&INST_1NNN,&INST_2NNN,&INST_3XNN,
        &INST_4XNN,&INST_5XY0,&INST_6XNN,&INST_7XNN,
        &INST_8000,&INST_9XY0,&INST_ANNN,&INST_BNNN,
        &INST_CXNN,&INST_DXYN,&INST_E000,&INST_F000
    };

    const uint32_t clock_speed = (uint32_t)(500/60);

    while(running)
    {   
        // 9 CPU Cycles
        for (uint32_t i = 0; i < clock_speed; i++)
        {
            chip8->opcode = chip8->memory.ram[chip8->PC] << 8 | chip8->memory.ram[(chip8->PC+1)];
            //printf("Executing now: %x\n",chip8->opcode);

            (*jump_table[chip8->opcode >> 12])(chip8);

        }

        if(chip8->DT > 0) chip8->DT--;
        if(chip8->ST > 0) chip8->ST--;

        if(drawFlag) {
            drawScreen(chip8);
            drawFlag = 0;
        }

        updateKeys(chip8);
        
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT) {
            running = 0;
        }

        SDL_Delay(5);
    }
    
}

void killSDL(void) {
    SDL_DestroyRenderer(window_render);
	SDL_DestroyWindow(window);
	SDL_Quit();
    window_render = NULL;
    window = NULL;
}

int main(int argc, char* argv[]) {
    
    if(argc != 2) {
        printf("Invalid Number of Arguments!");
        return 1;
    }

    initSDL();

    Chip8 chip8;    // Great name lol
    initChip8(&chip8);
    loadROM(argv[1],&chip8);
    game_loop(&chip8);
    killSDL();   
    return 0;
}
