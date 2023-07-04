#include <SDL.h>
#include <common.h>
#include <cpu.h>
#include <opcodes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REG_COUNT 16
#define MEM_SIZE 4096

static CPU cpu;

u8 memory[MEM_SIZE];
// 16 8-bit registers
u8 v[REG_COUNT];

int main(int argc, char **argv) {

  init_cpu(&cpu, argv[1]);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {

    printf("Failed to start SDL\n");
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, 640, 480, 0);

  if (window == NULL) {

    printf("Failed to create window\n");
    return -1;
  }

  SDL_Surface *surface = SDL_GetWindowSurface(window);

  if (surface == NULL) {

    printf("Failed to create surface from window\n");
    return -1;
  }

  // Fill the surface white
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

  SDL_UpdateWindowSurface(window);

  while (cpu.registers.PC < 0x200 + cpu.rom_size) {

    u8 buffer[2];

    // Copy 2 bytes from memory index of PC to buffer
    memcpy(buffer, &cpu.mem[cpu.registers.PC], 2);

    // Increment PC before handling OP code to make sure jumps etc work properly
    cpu.registers.PC += 2;
    get_opcode(&cpu, buffer);
  }

  // Hack to get window to stay up
  SDL_Event e;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
    }
  }

  SDL_DestroyWindow(window);

  SDL_Quit();

  return EXIT_SUCCESS;
}
