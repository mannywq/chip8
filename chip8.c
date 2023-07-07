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

u8 memory[MEM_SIZE];
// 16 8-bit registers
u8 v[REG_COUNT];

int main(int argc, char **argv) {

  // Our CPU object holding all Chip 8 registers and variables
  CPU *cpu = malloc(sizeof(CPU));

  // Initialise registers and SDL
  init_cpu(cpu, argv[1]);

  SDL_SetRenderDrawColor(cpu->renderer, 0, 0, 0, 0);

  SDL_RenderClear(cpu->renderer);

  SDL_RenderPresent(cpu->renderer);

  while (cpu->registers.PC < 0x200 + cpu->rom_size) {

    u8 buffer[2];

    // Copy 2 bytes from memory index of PC to buffer
    memcpy(buffer, &cpu->mem[cpu->registers.PC], 2);

    // Increment PC before handling OP code to make sure jumps etc work properly
    cpu->registers.PC += 2;

    uint32_t startFrame = SDL_GetTicks();

    get_opcode(cpu, buffer);

    uint32_t endFrame = SDL_GetTicks();

    uint32_t deltaTime = endFrame - startFrame;

    SDL_Delay(deltaTime > 16.67 ? deltaTime : 0);

    update_screen(cpu);
  }

  // Loop until window close button is pressed or the Esc key
  while (1) {

    if (SDL_PollEvent(&cpu->event) && cpu->event.type == SDL_QUIT) {
      break;
    } else if (cpu->event.type == SDL_KEYDOWN &&
               cpu->event.key.keysym.sym == SDLK_ESCAPE) {

      break;
    }
  }

  // Free SDL memory and cpu pointer
  kill_cpu(cpu);
  free(cpu);

  return EXIT_SUCCESS;
}
