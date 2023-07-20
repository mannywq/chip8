#pragma once

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define STACK_SIZE 16

#include <common.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

typedef struct {

  struct {

    u8 v[16];
    u16 I;
    u16 SP;
    u16 PC;

  } registers;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Event event;

  bool display[32][64];

  u8 cx;
  u8 cy;
  u8 mem[4096];
  int rom_size;
  u16 current_opcode;

  bool looping;
  bool isRunning;
  bool draw;
  bool wrapSprite;

  int scale;

u16 stack[16];

  u8 delay_timer, sound_timer;

  u8 halt;

  bool key[16];
  bool prev_key[16];
  char *rom_name;

} CPU;

void init_cpu(CPU *, char*);
void kill_cpu(CPU * cpu);

void update_screen(CPU *cpu);
void draw_pixel(CPU *cpu, int y, int x, bool pixel);
void push(CPU *cpu);
void pop(CPU* cpu);
void step();
