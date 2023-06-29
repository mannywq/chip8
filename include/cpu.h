#pragma once

#include "common.h"
#include <stdio.h>
#include <stdbool.h>


typedef struct {

  struct {

    u8 v[16];
    u16 I;
    u16 SP;
    u16 PC;

  } registers;

  u8 display[32][64];
  u8 mem[4096];
u8 rom_size;
u16 current_opcode;

u16 stack[16];

  u8 delay_timer, sound_timer;

  u8 halt;

  bool key[16];

  char *rom_name;

} CPU;

void init_cpu(CPU *, char*);

