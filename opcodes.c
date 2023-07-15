#include <common.h>
#include <cpu.h>
#include <stdio.h>

typedef void (*OpcodeFunction)(u16 opcode, CPU *cpu);

void opcode0(u16 opcode, CPU *cpu);
void opcode1(u16 opcode, CPU *cpu);
void opcode2(u16 opcode, CPU *cpu);
void opcode3(u16 opcode, CPU *cpu);
void opcode4(u16 opcode, CPU *cpu);
void opcode5(u16 opcode, CPU *cpu);
void opcode6(u16 opcode, CPU *cpu);
void opcode7(u16 opcode, CPU *cpu);
void opcode8(u16 opcode, CPU *cpu);
void opcode9(u16 opcode, CPU *cpu);
void opcodeA(u16 opcode, CPU *cpu);
void opcodeB(u16 opcode, CPU *cpu);
void opcodeC(u16 opcode, CPU *cpu);
void opcodeD(u16 opcode, CPU *cpu);
void opcodeE(u16 opcode, CPU *cpu);
void opcodeF(u16 opcode, CPU *cpu);

void opcode0(u16 opcode, CPU *cpu) {

  printf("Opcode function for %04x!\n", opcode);

  switch (opcode & 0x00FF) {

  case 0x00e0:

    printf("CLS - disp_clear()\n");

    SDL_SetRenderDrawColor(cpu->renderer, 0, 0, 0, 255);
    SDL_RenderClear(cpu->renderer);
    break;

  case 0x00ee:

    printf("RET - return from subroutine\n");
    break;
       

  default:
    printf("Unmatched op code %04x at $%04x. Might be data or sprite data\n",
           opcode, cpu->registers.PC);
    break;
  }
}

void opcode1(u16 opcode, CPU *cpu) {

  printf("1NNN - Jump/goto mem address %03x\n", opcode & 0x0FFF);

  cpu->registers.SP = opcode & 0x0fff;
}

void opcode2(u16 opcode, CPU *cpu) {

  printf("2NNN - Calls function/sub at $%03x\n", opcode & 0x0FFF);
}
void opcode3(u16 opcode, CPU *cpu) {

  printf("3XNN - if (Vx == NN) %#01x, %02x - skip if true\n",
         (opcode & 0x0F00) >> 8, opcode & 0x00FF);
}
void opcode4(u16 opcode, CPU *cpu) {

  u8 x = (opcode & 0x0F00) >> 8;
  u8 nn = opcode & 0x00FF;

  printf("4XNN - if (Vx != NN) %x, %02x - skip if true\n", x, nn);
}
void opcode5(u16 opcode, CPU *cpu) {

  u8 x = (opcode & 0x0F00) >> 8;
  u8 y = (opcode & 0x00F0) >> 4;

  printf("5XY0 - if reg x == y - V%01x, V%01x\n", x, y);
}

void opcode6(u16 opcode, CPU *cpu) {

  // Set VX, NN
  u8 x = (opcode & 0x0F00) >> 8;
  u8 nn = opcode & 0x00FF;

  printf("6XNN - SET Vx, NN\n");

  cpu->registers.v[x] = nn;

  printf("V[%d] is %02x\n", x, nn);
}

void opcode7(u16 opcode, CPU *cpu) {

  // ADD VX, NN
  u8 x = (opcode & 0x0F00) >> 8;
  u8 nn = opcode & 0x00FF;

  printf("7XNN - ADD VX, NN - %02x += %d\n", x, nn);

  printf("Initial value of V[%d] is %d\n", x, cpu->registers.v[x]);

  cpu->registers.v[x] += nn;

  printf("V[%d] is %d\n", x, cpu->registers.v[x]);
}

void opcode8(u16 opcode, CPU *cpu) {

  // Various register ops depending on last flag
  u8 x = (opcode & 0x0F00) >> 8;
  u8 y = (opcode & 0x00F0) >> 4;
  // u8 nn = opcode & 0x00FF;
  u8 o_flag = opcode & 0x000F;

  printf("8XYN = VX VY operation - %02x %02x flag: %01x\n", x, y, o_flag);

  // Extract instruction from last bit
  switch (o_flag) {

  case 0:

    printf("8XY0 - VX = VY - %02x %02x\n", x, y);

    cpu->registers.v[x] = cpu->registers.v[y];
    break;

  case 1:

    printf("OR VX, VY = VX |= VY - %02x %02x\n", x, y);

    cpu->registers.v[x] |= cpu->registers.v[y];
    break;
           
  case 2:

    printf("AND VX, VY = VX &= VY - %02x %02x\n", x, y);

    cpu->registers.v[x] &= cpu->registers.v[y];

    break;

  case 3:

    printf("XOR VX, VY = VX ^= VY - %02x %02x\n", x, y);

    cpu->registers.v[x] ^= cpu->registers.v[y];
    break;

  case 4:

    printf("ADD VX, VY += VY - %02x %02x (vx > 0xff ? v[0xf] = 1: 0\n", x, y);

    if (cpu->registers.v[x] + cpu->registers.v[y] > 0xff)
    {

      cpu->registers.v[0xf] = 1;

    }
    else 
      {

        cpu->registers.v[0xf] = 0;
      }

      cpu->registers.v[x] += cpu->registers.v[y] & 0xFF;

return;




    break;

  case 5:

    printf("SUB VX, VY = VX -= VY - %02x %02x (vx > vy ? 1 : 0)\n", x, y);

    if (cpu->registers.v[x] - cpu->registers.v[y] >= 0)
    {

      cpu->registers.v[0xf] = 1;

    }
    else 
      {

        cpu->registers.v[0xf] = 0;
      }

      cpu->registers.v[x] -= cpu->registers.v[y] & 0xFF;

return;

    break;

  case 6:

    // SHR VX (VY) - VY ignored in most implementations
    printf("SHR VX - VX >> 1 - %02x /2 - v[0xf] set to VX & 0x01\n", x);
    break;

  case 7:

    // Reverse of 5xy5 in order
    printf("SUBN VX, VY - VY - VX - %02x %02x (v[0xf] = vy < vx ? 1 : 0)\n", y,
           x);
    break;

  case 8:

    printf("SHL VX (VY) - VX <<= 1 - %02x *2\n - v[0xf] = VX & 0x01\n", x);
    break;

  default:

    printf("Unimplemented op code or data?\n");
    break;
  }
}

void opcode9(u16 opcode, CPU *cpu) {

  printf("9XY0 - SNE X, Y - vx !== vy ? pc+=2\n");
}

void opcodeA(u16 opcode, CPU *cpu) {

  printf("ANNN - SET MEM, NNN\n");

  cpu->registers.I = opcode & 0x0FFF;

  printf("I = $%04x\n", cpu->registers.I);
}

void opcodeB(u16 opcode, CPU *cpu) { printf("BNNN - JMP - PC = NNN + v0\n"); }
void opcodeC(u16 opcode, CPU *cpu) { printf("CXNN - Vx = rand() & NN\n"); }

void opcodeD(u16 opcode, CPU *cpu) {

  printf("DXYN - draw(vx, vy, N) - 8w * nh sprite at x, y coord\n");

  cpu->current_opcode = opcode;

  u8 xreg = (opcode & 0x0F00) >> 8;
  u8 yreg = (opcode & 0x00F0) >> 4;
  u8 n = opcode & 0x000F;

  u8 cx = cpu->registers.v[xreg]; 
  u8 cy = cpu->registers.v[yreg];

  cpu->registers.v[0xf] = 0;

  for (u8 i = 0; i < n; i++)
  {
    int draw_y = cy + i;
    if (cpu->wrapSprite == 1) draw_y &= DISPLAY_HEIGHT -1;
    if (draw_y >= DISPLAY_HEIGHT) break;


    u8 spriteByte = cpu->mem[cpu->registers.I + i];

    for (int j = 0; j < 8; j++)
    {
      int draw_x = cx + j;
      if (cpu->wrapSprite == 1) draw_x &= DISPLAY_WIDTH -1;
      if (draw_x >= DISPLAY_WIDTH) break;

      bool sprite_bit = (spriteByte >> (7 -j)) & 1;

      printf("%s", sprite_bit != 0 ? "|" : "*");

      if (sprite_bit == 0)
      {
        continue;
      }
      if (cpu->display[cy + i][cx +j] == 1 && sprite_bit == 1) {

        cpu->registers.v[0xf] = 1;

      }

      cpu->display[draw_y][draw_x] ^= sprite_bit;
    }
    
      puts("\n");
  }
    //Mark draw as true to make sure screen is redrawn
    cpu->draw = 1;
} //End function


void opcodeE(u16 opcode, CPU *cpu) 
{

  u8 x = (opcode & 0x0F00) >> 8;

  switch (opcode & 0x00FF) {

  case 0x009e:
    printf("EX9E - SE if key() == vx (%01x)\n", x);
    break;

  case 0x00a1:
    printf("EXA1 - SNE if key() != vx (%01x)\n", x);
    break;
  }
}

void opcodeF(u16 opcode, CPU *cpu) {

  u8 nn = opcode & 0x00FF;

  printf("NN is %02x\n", nn);

  switch (nn) {

  case 0x07:

    printf("FX07 - VX = get_delay() of timer\n");
    break;

  case 0x0a:

    printf("FX0A - Vx = get_key()\n");
    break;

  case 0x15:

    printf("delay_timer(Vx)\n");
    break;

  case 0x18:
    printf("sound_timer(Vx)\n");
    break;

  case 0x1e:

    printf("FX1E - MEM - I += Vx\n");
    break;
  case 0x29:
    printf("FX29 - MEM - I = sprite_addr[Vx]\n");
    break;

  case 0x33:

    printf("FX33 - BCD - I = bcd(vx)\n");
    break;

  case 0x55:
    printf("FX55 - reg_dump(vx, &i) - 0->vx stored in mem\n");
    break;

  case 0x65:
    printf("FX65 - reg_load(vx, &i) - load range until vx from i\n");
    break;
  default:

    printf("Unmatched op code %04x at $%04x. Might be data or sprite data\n",
           opcode, cpu->registers.PC);
  } // end switch (nn)
}

OpcodeFunction opcodeTable[16] = {
    [0] = opcode0,   [1] = opcode1,   [2] = opcode2,   [3] = opcode3,
    [4] = opcode4,   [5] = opcode5,   [6] = opcode6,   [7] = opcode7,
    [8] = opcode8,   [9] = opcode9,   [0xa] = opcodeA, [0xb] = opcodeB,
    [0xc] = opcodeC, [0xd] = opcodeD, [0xe] = opcodeE, [0xf] = opcodeF,
};

void get_opcode(CPU *cpu, u8 *buffer) {

  u8 msB = buffer[0];
  u8 lsB = buffer[1];

  u16 opcode = msB << 8 | lsB;

  printf("$%04x: (%04x) ", cpu->registers.PC, opcode);
  //  //  u8 reg = target & 0xFF00;
  //  //  u8 nn = target & 0x00FF;

  ////  u8 x = (opcode & 0x0F00);
  //  u8 y = (opcode & 0x00F0);
  //  u8 nn = (opcode & 0x00FF);

  //  u8 o_flag = opcode & 0x000F;

  u8 opcodePrefix = (opcode & 0xF000) >> 12;
  // printf("Reg x is %02x. Reg y is %02x\n", x, y);

  //  printf("Mem: $%04x - OP code: %04x \n", cpu->registers.PC, opcode);

  OpcodeFunction parseOpcode = opcodeTable[opcodePrefix];
  if (parseOpcode != NULL) {
    parseOpcode(opcode, cpu);
  }
}
