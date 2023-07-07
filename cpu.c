#include <cpu.h>
#include <stdio.h>
#include <string.h>

void init_cpu(CPU *cpu, char *file)
{


  if (SDL_Init(SDL_INIT_VIDEO) != 0) {

    printf("Failed to start SDL\n");
    exit(-1);
  }

  cpu->window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE);

  if (cpu->window == NULL) {

    printf("Failed to create window\n");
    exit(-1);

  }

  cpu->renderer = SDL_CreateRenderer(cpu->window, -1, 0);
  // SDL_Surface *surface = SDL_GetWindowSurface(window);

  if (cpu->renderer == NULL) {

    printf("Failed to create renderer\n");
    exit(-1);
  }


  FILE *fp = fopen(file, "rb");

  if (!fp)  {

    printf("Couldn't open file %s\n", file);
  }

  fseek(fp, 0, SEEK_END);

  cpu->rom_size = ftell(fp);

  rewind(fp);

  u8 font[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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
0xF0, 0x80, 0xF0, 0x80, 0x80  }; // F

u8 font_arr_size = sizeof(font)/sizeof(font[0]);

memcpy (cpu->mem, font, font_arr_size);

printf("%02x is the value of first font char in memory\n", cpu->mem[0]);
printf("$%04x: ", cpu->registers.PC);

//Set initial program counter offset
 cpu->registers.PC = 0x200;

 cpu->registers.SP = cpu->stack[STACK_SIZE -1];

  u8 bytes = fread(cpu->mem + 0x200, sizeof(u8), cpu->rom_size, fp);

 if (bytes != cpu->rom_size) {

    //printf("Loaded %d bytes\n", bytes);

    printf("Something went wrong loading memory\n");
  }

  printf("Loaded %d bytes\n", bytes);

  printf("File size is %d bytes\n", cpu->rom_size);

  cpu->rom_name = file;

  printf("Loaded Rom: %s\n", cpu->rom_name); 

  fclose(fp);

}

void kill_cpu(CPU *cpu)
{

  //Cleanup SDL windows and surfaces
  SDL_DestroyRenderer(cpu->renderer);
  SDL_DestroyWindow(cpu->window);

  SDL_Quit();

}

void update_screen(CPU *cpu)
{


  SDL_Rect rect = { .x = 0, .y = 0, .w = 20, .h = 20 };


  for (int i = 0; i < sizeof(cpu->display); i++)
  {

    rect.x = (i % 640) * rect.w;
    rect.y = (i / 640) * rect.h; 

    if (cpu->display[i])
    {
      printf("Plotting: %d\n ", cpu->display[i]);
      SDL_SetRenderDrawColor(cpu->renderer, 0xff, 0xff, 0xff, 0xff);
      SDL_RenderFillRect(cpu->renderer, &rect);

    }

  }

SDL_RenderPresent(cpu->renderer);

}

void push(CPU *cpu) {

  if (cpu->registers.SP > 0) {

    cpu->registers.SP--;
    cpu->stack[cpu->registers.SP] = cpu->registers.PC;
  }
  else {

    printf("Stack overflow!\n");
}

}

void pop(CPU *cpu) {

  cpu->registers.PC = cpu->stack[cpu->registers.SP];

  if (cpu->registers.SP < STACK_SIZE -1) {

    cpu->registers.SP++;
  }

}







