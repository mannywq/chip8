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
#define INST_PER_FRAME 10

u8 memory[MEM_SIZE];
// 16 8-bit registers
u8 v[REG_COUNT];

int main(int argc, char **argv) {

  // Our CPU object holding all Chip 8 registers and variables
  CPU *cpu = malloc(sizeof(CPU));

  // Initialise registers and SDL
  init_cpu(cpu, argv[1]);

  while (cpu->registers.PC < 0x200 + cpu->rom_size && cpu->looping != 1) {

    // uint32_t startFrame = SDL_GetTicks();

    // for (int i = 0; i < INST_PER_FRAME; i++) {

    get_opcode(cpu);
    //}

    if (cpu->draw == 1) {
      puts("Draw flag set\n");
      update_screen(cpu);

      cpu->draw = 0;
    }
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
