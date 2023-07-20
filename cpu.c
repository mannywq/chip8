#include <cpu.h>
#include <stdio.h>
#include <string.h>

void init_cpu(CPU *cpu, char *file)
{
  memset(cpu, 0, sizeof(CPU));

  cpu->looping = 0;
  cpu->scale = 10;
  cpu->wrapSprite = 1;

  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
      cpu->display[y][x] = 0;
    }
  }


  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {

    printf("Failed to start SDL\n");
    exit(-1);
  }

  cpu->window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH * cpu->scale, DISPLAY_HEIGHT * cpu->scale, SDL_WINDOW_SHOWN);

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

//  cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELTYPE_INDEX8, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  //SDL_RenderPresent(cpu->renderer);


  
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
printf("Memory size: %lu\n", sizeof(cpu->mem));

//Set initial program counter offset
 cpu->registers.PC = 0x200;


printf("Program counter: $%04x\n", cpu->registers.PC);

 cpu->registers.SP = STACK_SIZE -1;

FILE *fp = fopen(file, "rb");

  if (!fp)  {

    printf("Couldn't open file %s\n", file);
  }

  fseek(fp, 0, SEEK_END);

  cpu->rom_size = ftell(fp);

  fseek(fp, 0, SEEK_SET);


  int bytes = fread(&cpu->mem[0x200], sizeof(u8), cpu->rom_size, fp);

 if (bytes != cpu->rom_size) {


    printf("Something went wrong loading memory\n");
  }

  printf("Loaded %d bytes\n", bytes);

  printf("File size is %d bytes\n", cpu->rom_size);

  cpu->rom_name = file;

  printf("Loaded Rom: %s\n", cpu->rom_name); 

  fclose(fp);

  cpu->isRunning = 1;

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
  SDL_RenderClear(cpu->renderer);

  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {

    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {

      bool pixel = cpu->display[y][x]; 
      draw_pixel(cpu, y, x, pixel);
      
    }

  }

  SDL_RenderPresent(cpu->renderer);
} //End update_screen()


void draw_pixel(CPU *cpu, int y, int x, bool pixel)
{
  
  SDL_Rect rect = { .x = x * cpu->scale, .y = y * cpu->scale, .w = cpu->scale, .h = cpu->scale };


  if (pixel == 1)
  {

    //printf("Drawing rect at y: %d x: %d and color: %d\n", rect.y, rect.x, pixel);
    SDL_SetRenderDrawColor(cpu->renderer, 255, 255, 255, 255);
  }
  else 
  {
//  printf("Drawing rect at y: %d x: %d and color: %d\n", rect.y, rect.x, pixel);
    SDL_SetRenderDrawColor(cpu->renderer, 35,35,35, 255);
  }



  SDL_RenderFillRect(cpu->renderer, &rect);

}

void push(CPU *cpu) {

  if (cpu->registers.SP > 0) {

    cpu->registers.SP--;
    cpu->stack[cpu->registers.SP] = cpu->registers.PC;

    printf("Stack pointer is now %02x\n", cpu->stack[cpu->registers.SP]);
  }
  else {

    printf("Stack overflow!\n");
}

}

void pop(CPU *cpu) {

  cpu->registers.PC = cpu->stack[cpu->registers.SP];

printf("Program counter is now %02x\n", cpu->registers.PC);

  if (cpu->registers.SP < STACK_SIZE -1) {

    cpu->registers.SP++;
  }

}

void step() {

  while(1) {

    SDL_Event ev;

    if (SDL_PollEvent(&ev) && ev.type == SDL_KEYDOWN) {

      if (ev.key.keysym.sym == SDLK_KP_SPACE) break;

    }

  }

}






