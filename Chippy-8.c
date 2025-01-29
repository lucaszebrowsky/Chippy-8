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

#include "Chippy-8.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Initial Scale. Change this to whatever you prefer
#define SCALE 15

SDL_Window *window = NULL;
SDL_Renderer *window_render = NULL;
u8 drawFlag = 0;

const int keymap[16] = {
	SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
	SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
	SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
	SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V,
};

void initSDL(void)
{
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		printf("SDL Init Error!");
		exit(1);
	}

	window = SDL_CreateWindow("Chippy-8", 64 * SCALE, 32 * SCALE,
							  SDL_WINDOW_RESIZABLE);

	if (!window) {
		printf("Failed to create SDL Window");
		exit(1);
	}

	window_render = SDL_CreateRenderer(window, NULL);

	if (!window_render) {
		printf("Failed to create SDL Renderer!");
		exit(1);
	}
}

void initChip8(Chip8 *chip8)
{
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
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	// Clearing RAM
	memset(chip8->memory.ram, 0, sizeof(chip8->memory.ram));

	// Clearing VRAM
	memset(chip8->display, 0, sizeof(chip8->display));

	// Clearing Register
	memset(chip8->V, 0, sizeof(chip8->V));

	// Clearing Keyboard
	memset(chip8->keyboard, 0, sizeof(chip8->keyboard));

	// Loading Fonts into Memory
	memcpy(chip8->memory.fonts, font_array, sizeof(font_array));

	chip8->PC = 0x200; // Start of Program Code
	chip8->SP = 0;
	chip8->Index = 0;
	chip8->DT = 60;
	chip8->ST = 60;
}

void loadROM(char *path, Chip8 *chip8)
{
	FILE *file = fopen(path, "rb");
	size_t file_size = 0;
	if (file == NULL) {
		printf("Error opening ROM!\n");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size > sizeof(chip8->memory.rom)) {
		printf("File too big! Max ROM Size is %zu bytes!\n",
			   sizeof(chip8->memory.rom));
		exit(1);
	}

	size_t result = fread(chip8->memory.rom, file_size, 1, file);

	if (result != 1) {
		printf("Could not read ROM into memory!\n");
		exit(1);
	}

	fclose(file);
	file = NULL;
}

void drawScreen(Chip8 *chip8)
{
	SDL_SetRenderDrawColor(window_render, 0, 0, 0, 255);
	SDL_RenderClear(window_render);
	SDL_SetRenderDrawColor(window_render, 0, 255, 0, 255);

	int32_t width = 0;
	int32_t height = 0;

	SDL_GetWindowSize(window, &width, &height);

	uint32_t scalex = width / 64;
	uint32_t scaley = height / 32;

	for (u8 py = 0; py < 32; py++) { // y

		for (u8 px = 0; px < 64; px++) { // x

			if (chip8->display[px][py]) {
				SDL_FRect rect;
				rect.x = px * scalex;
				rect.y = py * scaley;
				rect.w = scalex;
				rect.h = scaley;

				SDL_RenderFillRect(window_render, &rect);
			}
		}
	}

	SDL_RenderPresent(window_render);
}

/*
 *   Original Chip8 Keyboard Layout:
 *
 *   1   2   3   C
 *   4   5   6   D
 *   7   8   9   E
 *   A   0   B   F
 *
 *   Translated Keyboard Layout:
 *
 *   1   2   3   4
 *   Q   W   E   R
 *   A   S   D   F
 *   Y   X   C   V
 */
void updateKeys(Chip8 *chip8)
{
	/*
     *   This function returns a pointer to an SDL internal array,
     *   so it will always return the same pointer and wont cause a memory leak
     */
	const _Bool *key_state = SDL_GetKeyboardState(NULL);
	SDL_PumpEvents();

	for (u8 i = 0; i < 0x10; i++) {
		chip8->keyboard[i] = key_state[keymap[i]];
	}
}

/*
 *  00E0 -> Clear Screen(VRAM)
 *  00EE -> Return from subroutine
 */
void INST_0000(Chip8 *chip8)
{
	switch (chip8->opcode) {
	case 0x00E0:
		memset(chip8->display, 0, sizeof(chip8->display));
		break;

	case 0x00EE: // RET
		chip8->PC = chip8->Stack[chip8->SP];
		chip8->SP--;
		break;

	default:
		printf("Error! Opcode: 0x%04x at PC: %04d\n", chip8->opcode, chip8->PC);
		exit(1);
		break;
	}
	chip8->PC += 2;
}

// Jump to address nnn (set PC to nnn)
void INST_1NNN(Chip8 *chip8)
{
	chip8->PC = (chip8->opcode & 0x0FFF);
}

// Execute subroutine at address nnn
void INST_2NNN(Chip8 *chip8)
{
	chip8->SP++;
	chip8->Stack[chip8->SP] = chip8->PC;
	chip8->PC = (chip8->opcode & 0x0FFF);
}

// Skip the next instruction if v[x] == nn
void INST_3XNN(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 nn = (chip8->opcode & 0x00FF);

	if (chip8->V[x] == nn) {
		chip8->PC += 2;
	}
	chip8->PC += 2;
}

// Skip the next instruction if v[x] != nn
void INST_4XNN(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 nn = (chip8->opcode & 0x00FF);

	if (chip8->V[x] != nn) {
		chip8->PC += 2;
	}
	chip8->PC += 2;
}

// Skip the next instruction if v[x] == v[y]
void INST_5XY0(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 y = (chip8->opcode & 0x00F0) >> 4;

	if (chip8->V[x] == chip8->V[y]) {
		chip8->PC += 2;
	}
	chip8->PC += 2;
}

// Store nn in v[x]
void INST_6XNN(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 nn = (chip8->opcode & 0x00FF);

	chip8->V[x] = nn;
	chip8->PC += 2;
}

// Add nn to v[x]
void INST_7XNN(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 nn = (chip8->opcode & 0x00FF);

	chip8->V[x] += nn;
	chip8->PC += 2;
}

/*
 * 8XY0 -> Store v[y] in v[x]
 * 8XY1 -> Set v[x] to v[x] OR v[y](bitwise)
 * 8XY2 -> Set v[x] to v[x] AND v[y](bitwise)
 * 8XY3 -> Set v[x] to v[x] XOR v[y](bitwise)
 * 8XY4 -> Add v[y] to v[x] with carry-checking(v[0xF])
 * 8XY5 -> Substract v[y] from v[x] with carry-checking(v[0xF])
 * 8XY6 -> Store v[y] shifted right one bit in v[x],v[0xF] is set to the least
 * significant bit prior to the shift 8XY7 -> Set v[x] = v[y] - v[x] with
 * carry-checking 8XYE -> Store v[y] shifted left one bit in v[x],v[0xF] is set
 * to the most significant bit prior to the shift
 */
void INST_8000(Chip8 *chip8)
{
	u16 foo = (chip8->opcode & 0x000F);
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 y = (chip8->opcode & 0x00F0) >> 4;

	switch (foo) {
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
		chip8->V[0xF] = ((chip8->V[x] + chip8->V[y]) > 0xFF);
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
		chip8->V[0xF] = chip8->V[x] >> 7;
		chip8->V[x] = chip8->V[x] << 1;
		break;

	default:
		printf("Error! Opcode: 0x%04x at PC: %04d\n", chip8->opcode, chip8->PC);
		exit(1);
		break;
	}
	chip8->PC += 2;
}

// Skip the next instruction if v[x] != v[y]
void INST_9XY0(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 y = (chip8->opcode & 0x00F0) >> 4;

	if (chip8->V[x] != chip8->V[y]) {
		chip8->PC += 2;
	}
	chip8->PC += 2;
}

// Store nnn in Index-Register
void INST_ANNN(Chip8 *chip8)
{
	u16 nnn = (chip8->opcode & 0x0FFF);

	chip8->Index = nnn;
	chip8->PC += 2;
}

// Set PC to nnn + v[0]
void INST_BNNN(Chip8 *chip8)
{
	u16 nnn = (chip8->opcode & 0x0FFF);

	chip8->PC = chip8->V[0] + nnn;
}

// Set v[x] to a random number with a mask of nn
void INST_CXNN(Chip8 *chip8)
{
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 nn = (chip8->opcode & 0xFF);

	chip8->V[x] = (rand() % 256) & nn;
	chip8->PC += 2;
}

// Draw a sprite at position v[x], v[y] with n bytes of sprite data starting at
// the address stored in Index
void INST_DXYN(Chip8 *chip8)
{
	chip8->V[0xF] = 0;
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	u8 y = (chip8->opcode & 0x00F0) >> 4;
	u8 height = chip8->opcode & 0x000F;
	u8 pixel = 0;

	for (u16 yLine = 0; yLine < height; yLine++) {
		pixel = chip8->memory.ram[chip8->Index + yLine];
		for (u16 xLine = 0; xLine < 8; xLine++) {
			if ((pixel & (0x80 >> xLine)) != 0) {
				if (chip8->display[(chip8->V[x] + xLine) % 64]
								  [(chip8->V[y] + yLine) % 32] == 1) {
					chip8->V[0xF] = 1;
				}
				chip8->display[(chip8->V[x] + xLine) % 64]
							  [(chip8->V[y] + yLine) % 32] ^= 1;
			}
		}
	}
	drawFlag = 1;
	chip8->PC += 2;
}

/*
 * EX9E -> Skip the next instruction if the key corresponding to the hex
 * value currently stored in register v[x] is pressed EXA1 -> Skip the next
 * instruction if the key corresponding to the hex value currently stored in
 * register v[x] is not pressed
 */
void INST_E000(Chip8 *chip8)
{
	u8 foo = (chip8->opcode & 0x00FF);
	u8 x = (chip8->opcode & 0x0F00) >> 8;

	switch (foo) {
	case 0x009E:

		if (chip8->keyboard[chip8->V[x]]) {
			chip8->PC += 2;
		}
		break;

	case 0x00A1:

		if (!chip8->keyboard[chip8->V[x]]) {
			chip8->PC += 2;
		}
		break;

	default:
		printf("Error! Opcode: 0x%04x at PC: %04d\n", chip8->opcode, chip8->PC);
		exit(1);
		break;
	}
	chip8->PC += 2;
}

/*
 * FX07 -> Store Delay Timer(DT) in v[x]
 * FX0A -> Wait for a keypress and store the result in v[x]
 * FX15 -> Set the Delay Timer(DT) to v[x]
 * FX18 -> Set the Sound Timer(ST) to v[x]
 * FX1E -> Add v[x] to Index-Register
 * FX29 -> Set Index-Register to the memory address of the sprite data
 * corresponding to the hexadecimal digit stored in register v[x] FX33 -> Store
 * BCD representation of v[x] in memory locations I, I+1, and I+2 FX55 -> Store
 * the values of registers v[0] to v[0xF] inclusive in memory starting at
 * address I. FX65 -> Fill registers v[0] to v[x] inclusive with the values
 * stored in memory starting at address I
 */
void INST_F000(Chip8 *chip8)
{
	u8 foo = (chip8->opcode & 0x00FF);
	u8 x = (chip8->opcode & 0x0F00) >> 8;
	const _Bool *key_state = NULL;

	switch (foo) {
	case 0x0007:
		chip8->V[x] = chip8->DT;
		break;

	case 0x000A:
		key_state = SDL_GetKeyboardState(NULL);

		// Wait for Keypress by user,only valid keys count!
		while (1) {
			SDL_PumpEvents();
			for (u8 i = 0; i < 0x10; i++) {
				if (key_state[keymap[i]]) {
					chip8->V[x] = i;
					goto DONE; // get out of this shit
				}
			}
			SDL_Delay(1);
		}
DONE:
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
		/*
         * V[x] can contain 0x0 - 0xF.
         * These are stored in the font region in memory(starting at 0x00) and are
         * all 5 bytes long, so in order to get the correct starting address we have
         * to multiply the value by 5 Example: Draw hex 3 -> 0x3 * 0x5 = 0xF -> font
         * address of hex 3 starts at 0xF
         */
		chip8->Index = chip8->V[x] * 0x5;
		break;

	case 0x0033:
		chip8->memory.ram[chip8->Index] = (chip8->V[x] / 100);
		chip8->memory.ram[chip8->Index + 1] = ((chip8->V[x] / 10) % 10);
		chip8->memory.ram[chip8->Index + 2] = (chip8->V[x] % 100) % 10;
		break;

	case 0x0055:
		for (u8 i = 0; i <= x; i++) {
			chip8->memory.ram[(chip8->Index + i)] = chip8->V[i];
		}
		break;

	case 0x0065:
		for (u8 i = 0; i <= x; i++) {
			chip8->V[i] = chip8->memory.ram[(chip8->Index + i)];
		}
		break;

	default:
		printf("Error! Opcode: 0x%04x at PC: %04d\n", chip8->opcode, chip8->PC);
		exit(1);
		break;
	}

	chip8->PC += 2;
}

void game_loop(Chip8 *chip8)
{
	SDL_Event event;
	u8 running = 1;
	// 500 Instructions/60 Hz == ~ 8 Cycles
	const uint32_t clock_speed = (uint32_t)(500 / 60);

	// Sound Stuff
	SDL_AudioSpec wavSpec;
	uint32_t wavLength;
	u8 *wavBuffer;

	// Jump Table
	void (*jump_table[16])(Chip8 *) = {
		INST_0000, INST_1NNN, INST_2NNN, INST_3XNN, INST_4XNN, INST_5XY0,
		INST_6XNN, INST_7XNN, INST_8000, INST_9XY0, INST_ANNN, INST_BNNN,
		INST_CXNN, INST_DXYN, INST_E000, INST_F000
	};

	/*
   	 * If you want to use your own sound file, just place your .wav in the same
   	 * directory as the executable. Keep in mind that you need to rename it to
	 * "sound.wav"!
   	 */
	SDL_LoadWAV("sound.wav", &wavSpec, &wavBuffer, &wavLength);
	SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wavSpec, NULL, NULL);

	while (running) {
		// 8 CPU Cycles
		for (uint32_t i = 0; i < clock_speed; i++) {
			chip8->opcode = chip8->memory.ram[chip8->PC] << 8 |
							chip8->memory.ram[(chip8->PC + 1)];

			(*jump_table[chip8->opcode >> 12])(chip8);
		}

		if (chip8->DT > 0)
			chip8->DT--;

		if (chip8->ST > 0) {
			if (chip8->ST == 1) {
				SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
				SDL_PutAudioStreamData(stream, wavBuffer, wavLength);
			}
			chip8->ST--;
		}

		if (drawFlag) {
			drawScreen(chip8);
			drawFlag = 0;
		}

		updateKeys(chip8);

		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) {
			running = 0;
		}
		SDL_Delay(5);
	}

	SDL_CloseAudioDevice(SDL_GetAudioStreamDevice(stream));
	SDL_free(wavBuffer);
}

void killSDL(void)
{
	SDL_DestroyRenderer(window_render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	window_render = NULL;
	window = NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Invalid Number of Arguments!");
		return 1;
	}

	initSDL();
	srand(time(NULL)); // Seed the Random number generator
	Chip8 chip8; // Great name lol
	initChip8(&chip8);
	loadROM(argv[1], &chip8);
	game_loop(&chip8);
	killSDL();
	return 0;
}
