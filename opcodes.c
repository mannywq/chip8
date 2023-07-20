#include <common.h>
#include <cpu.h>
#include <stdio.h>

typedef void (*OpcodeFunction)(u16 opcode, CPU *cpu);

bool reg_flag = 0;

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

  switch (opcode & 0x0FFF) {

  case 0x0e0:

    printf("CLS - disp_clear()\n");

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
          cpu->display[y][x] = 0;
        }
    }

    cpu->draw = 1;


    break;

    
  case 0x0ee:

    printf("RET - return from subroutine\n");

    pop(cpu);

    break;
       

  default:
    printf("Unmatched op code %04x at $%04x. Might be data or sprite data\n",
           opcode, cpu->registers.PC);
    break;
  }
}

void opcode1(u16 opcode, CPU *cpu) {

  printf("1NNN - Jump/goto mem address %03x\n", opcode & 0x0FFF);

  u16 destination = opcode & 0x0fff;

  if (destination != cpu->registers.PC)
  {
    cpu->registers.PC = destination;
  }
  else 
  {

    cpu->looping = 1;
    puts("Looping");

  }
}


void opcode2(u16 opcode, CPU *cpu) {

  printf("2NNN - Calls function/sub at $%03x\n", opcode & 0x0FFF);

  u16 call = opcode & 0x0FFF;

  push(cpu);

  cpu->registers.PC = call;


}
void opcode3(u16 opcode, CPU *cpu) {
  u8 rx = (opcode & 0x0F00) >> 8;
  u8 nn = (opcode & 0x00FF);

  printf("3XNN - if (Vx == NN) %02x, %02x - skip if true\n",
         cpu->registers.v[rx], opcode & 0x00FF);


  if (cpu->registers.v[rx] == nn) cpu->registers.PC +=2;
}
void opcode4(u16 opcode, CPU *cpu) {

  u8 x = (opcode & 0x0F00) >> 8;
  u8 nn = opcode & 0x00FF;

  if (cpu->registers.v[x] != nn) cpu->registers.PC += 2;

  printf("4XNN - if (Vx != NN) %x, %02x - skip if true\n", cpu->registers.v[x], nn);
}
void opcode5(u16 opcode, CPU *cpu) {

  u8 x = (opcode & 0x0F00) >> 8;
  u8 y = (opcode & 0x00F0) >> 4;

  printf("5XY0 - if reg x == y - %02x, %01x\n", cpu->registers.v[x], cpu->registers.v[y]);

  if (cpu->registers.v[x] == cpu->registers.v[y]) cpu->registers.PC += 2;
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

  printf("7XNN - ADD VX, NN - Reg %01x (%02x) += %d\n", x, cpu->registers.v[x], nn);

  printf("Initial value of V[%d] is %d\n", x, cpu->registers.v[x]);

  cpu->registers.v[x] += nn & 0xFF;

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

    if (cpu->registers.v[x] += cpu->registers.v[y] > 0xff)
    {

     reg_flag = 1; 

    }
    else 
    {

        reg_flag = 0;
    }

    cpu->registers.v[x] += cpu->registers.v[y] & 0xFF;

    cpu->registers.v[0xf] = reg_flag;

return;




    break;

  case 5:

    printf("SUB VX, VY = VX -= VY - %02x %02x (vx > vy ? 1 : 0)\n", x, y);

    if (cpu->registers.v[x] > cpu->registers.v[y])
    {

      cpu->registers.v[0xf] = 1;

    }
    else 
      {

        cpu->registers.v[0xf] = 0;
      }

      cpu->registers.v[x] -= cpu->registers.v[y];

return;

    break;

  case 6:

    // SHR VX (VY) - VY ignored in most implementations
    printf("SHR VX - VX >> 1 - %02x /2 - v[0xf] set to VX & 0x01\n", x);

    //Check if least significant bit is set.
    if (cpu->registers.v[x] & 1) reg_flag = 1;

    cpu->registers.v[x] >>= 1;

    break;

  case 7:

    // Reverse of 5xy5 in order
    printf("SUBN VX, VY - VY - VX - %02x %02x (v[0xf] = vy < vx ? 1 : 0)\n", y,x);

    if (cpu->registers.v[y] > cpu->registers.v[x])
    {

      cpu->registers.v[0xf] = 1;

    }
    else 
      {

        cpu->registers.v[0xf] = 0;
      }


    cpu->registers.v[x] = cpu->registers.v[y] - cpu->registers.v[x];

    break;

  case 0xe:

    printf("SHL VX (VY) - VX <<= 1 - %02x *2\n - v[0xf] = VX & 0x01\n", x);

    if ((cpu->registers.v[x] >> 7) & 1) reg_flag = 1;
    else reg_flag = 0;

    cpu->registers.v[x] <<= 1;

    break;

  default:

    printf("Unimplemented op code or data?\n");
    break;
  }
}

void opcode9(u16 opcode, CPU *cpu) {

  printf("9XY0 - SNE X, Y - vx !== vy ? pc+=2\n");

  u8 vx = 0x0F00 >> 8;
  u8 vy = 0x00F0 >> 4;

  if (cpu->registers.v[vx] != cpu->registers.v[vy]) cpu->registers.PC +=2;
}


void opcodeA(u16 opcode, CPU *cpu) {

  printf("ANNN - SET MEM, NNN\n");

  cpu->registers.I = (opcode & 0x0FFF);

  printf("I = $%04x\n", cpu->registers.I);
}

void opcodeB(u16 opcode, CPU *cpu) { 

  printf("BNNN - JMP - PC = NNN + v0\n"); 

  u16 dest = (opcode & 0x0FFF) + cpu->registers.v[0];
  cpu->registers.PC = dest; 

}
void opcodeC(u16 opcode, CPU *cpu) { printf("CXNN - Vx = rand() & NN\n"); }

void opcodeD(u16 opcode, CPU *cpu) {


  cpu->current_opcode = opcode;

  u8 rx = (opcode & 0x0F00) >> 8;
  u8 ry = (opcode & 0x00F0) >> 4;
  u8 n = opcode & 0x000F;

  u8 vx = cpu->registers.v[rx]; 
  u8 vy = cpu->registers.v[ry];
  
  printf("DXYN - draw(Reg x, Reg y, N rows) - 8w * %d sprite from %04x at %d, %d coord\n", n, cpu->registers.I, vx, vy);

  u8 vf = 0;

  //Row loop
  for (int y = 0; y < n; y++)
  {
        u8 spriteByte = cpu->mem[cpu->registers.I + y];

        u8 draw_y = vy + y >= DISPLAY_HEIGHT ? (vy +y) % DISPLAY_HEIGHT : vy +y;

    for (int x = 0; x < 8; x++)
    {
      u8 draw_x = (vx + x) >= DISPLAY_WIDTH ? (vx + x) % DISPLAY_WIDTH : vx + x;

      bool sprite_bit = spriteByte >> (7 - x) & 1;


      bool set = cpu->display[draw_y][draw_x];

      if (set && sprite_bit)
      {
        vf = 1;

      }
      {
        cpu->display[draw_y][draw_x] ^= sprite_bit;

      }
     // }

//      printf("%s", sprite_bit != 0 ? "|" : "*");
    }
    
  }

  cpu->registers.v[0xf] = vf;
    //Mark draw as true to make sure screen is redrawn
    cpu->draw = 1;
} //End function


void opcodeE(u16 opcode, CPU *cpu) 
{

  u8 x = (opcode & 0x0F00) >> 8;

  switch (opcode & 0x00FF) {

  case 0x009e:
    printf("EX9E - SE if key() == vx (%01x)\n", x);

        
      if (cpu->key[cpu->registers.v[x] & 0xf] == 1)

          cpu->registers.PC += 2;


    break;

  case 0x00a1:
    printf("EXA1 - SNE if key() != vx (%01x)\n", x);

    if (cpu->key[cpu->registers.v[x] & 0xf] != 1)

          cpu->registers.PC += 2;

        break;
  }
}

void opcodeF(u16 opcode, CPU *cpu) {

  u8 x = (opcode & 0x0F00) >> 8;
  u8 nn = opcode & 0x00FF;

  //printf("NN is %02x\n", nn);

  switch (nn) {

  case 0x07:

    printf("FX07 - VX = get_delay() of timer\n");

      cpu->registers.v[x] = cpu->delay_timer;
    break;

  case 0x0a:

    printf("FX0A - Vx = get_key()\n");

        bool keyPressed = false;

        for (int i = 0; i < 16; i++)
        {
          if (cpu->prev_key[i] != 0) {

            if (cpu->key[i] == 0) {
              cpu->registers.v[x] = i;
          keyPressed = true;
              break;
          }
          }


        }

        if (!keyPressed) cpu->registers.PC -= 2;
        
          
    break;

  case 0x15:

    printf("delay_timer(Vx)\n");

    cpu->delay_timer = cpu->registers.v[x];
    break;

  case 0x18:
    printf("sound_timer(Vx)\n");

    cpu->sound_timer = cpu->registers.v[x];
    break;

  case 0x1e:

    printf("FX1E - MEM - I += Vx\n");

    cpu->registers.I += cpu->registers.v[x];

    printf("I now pointing at %04x\n", cpu->registers.I);

    break;
  case 0x29:
    printf("FX29 - MEM - I = sprite_addr[Vx]\n");

    cpu->registers.I = cpu->mem[cpu->registers.v[x]];

    break;

  case 0x33:

    printf("FX33 - BCD - I = bcd(vx)\n");

    u8 num = cpu->registers.v[x];

    u8 ones = num % 10;
    num /= 10;
    u8 tens = num % 10;
    num /= 10;
    u8 hundreds = num;

    cpu->mem[cpu->registers.I] = hundreds;
    cpu->mem[cpu->registers.I + 1] = tens;
    cpu->mem[cpu->registers.I + 2] = ones;


    break;

  case 0x55:
    printf("FX55 - reg_dump(vx, &i) - 0->vx stored in mem\n");

    for (int i = 0; i <= x; i++) {

        cpu->mem[cpu->registers.I + i] = cpu->registers.v[i];
    }
    break;

  case 0x65:
    printf("FX65 - reg_load(vx, &i) - load range until vx from i\n");

    for (int i = 0; i <= x; i++) {

        cpu->registers.v[i] = cpu->mem[cpu->registers.I + i];

    }

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

void get_opcode(CPU *cpu) {


    u8 buffer[2];

    // Copy 2 bytes from memory index of PC to buffer
    memcpy(buffer, &cpu->mem[cpu->registers.PC], 2);


  u8 msB = buffer[0];
  u8 lsB = buffer[1];

  u16 opcode = msB << 8 | lsB;

  if (opcode == 0x0000) {

    cpu->isRunning = 0;
    puts("Out of data");
  }

  printf("$%04x: (%04x) ", cpu->registers.PC, opcode);
  
  u8 opcodePrefix = (opcode & 0xF000) >> 12;


    cpu->registers.PC += 2;

  OpcodeFunction parseOpcode = opcodeTable[opcodePrefix];

  if (parseOpcode != NULL) {
    parseOpcode(opcode, cpu);
  }


}
