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

  while (cpu->isRunning) {

    bool keyState;

    for (int i = 0; i < 16; i++) {

      cpu->prev_key[i] = cpu->key[i];
    }

    while (SDL_PollEvent(&cpu->event) != 0) {

      if (cpu->event.type == SDL_QUIT)
        break;

      else if (cpu->event.type == SDL_KEYDOWN) {

        keyState = 1;

        printf("%s\n", SDL_GetKeyName(cpu->event.key.keysym.sym));

      } else if (cpu->event.type == SDL_KEYUP)
        keyState = 0;

      switch (cpu->event.key.keysym.sym) {

      case SDLK_ESCAPE:

        cpu->isRunning = 0;

        break;

      case SDLK_0:

        cpu->key[0] = keyState;

        break;

      case SDLK_1:

        cpu->key[1] = keyState;

        break;

      case SDLK_2:

        cpu->key[2] = keyState;

        break;

      case SDLK_3:

        cpu->key[3] = keyState;

        break;

      case SDLK_4:

        cpu->key[4] = keyState;

        break;
      case SDLK_5:

        cpu->key[5] = keyState;

        break;

      case SDLK_6:

        cpu->key[6] = keyState;

        break;
      case SDLK_7:

        cpu->key[7] = keyState;

        break;
      case SDLK_8:

        cpu->key[8] = keyState;

        break;
      case SDLK_9:
        cpu->key[9] = keyState;

        break;
      case SDLK_a:

        cpu->key[0xa] = keyState;

        break;
      case SDLK_b:

        cpu->key[0xb] = keyState;
        break;
      case SDLK_c:
        cpu->key[0xc] = keyState;
        break;
      case SDLK_d:
        cpu->key[0xd] = keyState;
        break;
      case SDLK_e:
        cpu->key[0xe] = keyState;
        break;
      case SDLK_f:
        cpu->key[0xf] = keyState;
        break;
      }
    }

    double startTime = SDL_GetTicks();

    if (cpu->delay_timer > 0)
      cpu->delay_timer--;

    if (cpu->sound_timer > 0) {

      cpu->sound_timer--;
      printf("\a");
    }

    for (int i = 0; i < INST_PER_FRAME; i++) {

      get_opcode(cpu);
    }

    if (cpu->draw == 1) {
      puts("Draw flag set\n");
      update_screen(cpu);

      cpu->draw = 0;
    }

    double finishTime = SDL_GetTicks();

    double deltaTime = finishTime - startTime;

    double sleepTime = 16.67 - deltaTime;

    printf("Delta time: %f\n", sleepTime);

    if (sleepTime > 0)
      SDL_Delay(sleepTime);

    else
      SDL_Delay(16);
  }

  // Loop until window close button is pressed or the Esc key
  /*while (1) {

    if (SDL_PollEvent(&cpu->event) && cpu->event.type == SDL_QUIT) {
      break;
    } else if (cpu->event.type == SDL_KEYDOWN &&
               cpu->event.key.keysym.sym == SDLK_ESCAPE) {

      break;
    }
  }*/

  // Free SDL memory and cpu pointer
  kill_cpu(cpu);
  free(cpu);

  return EXIT_SUCCESS;
}
