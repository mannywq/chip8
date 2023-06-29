#include <stdio.h>
#include <common.h>
#include <cpu.h>

typedef void (*OpcodeFunction) (u16 opcode, CPU *cpu);

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
      break;

    case 0x00ee:

      printf("RET - return from subroutine\n");
      break;

    default:
      printf("Unknown opcode - maybe data? %04x\n", opcode);
      break;
    }

}

void opcode1(u16 opcode, CPU *cpu) {

    printf("1NNN - Jump/goto mem address %03x\n", opcode & 0x0FFF);


}

void opcode2(u16 opcode, CPU *cpu) {

    printf("2NNN - Calls function/sub at $%03x\n", opcode & 0x0FFF);



}
void opcode3(u16 opcode, CPU *cpu) {

printf("3XNN - if (Vx == NN) %#01x, %02x - skip if true\n", (opcode & 0x0F00) >> 8, opcode & 0x00FF);


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

//Set VX, NN
u8 x = (opcode & 0x0F00) >> 8;
u8 nn = opcode & 0x00FF;

    printf("6XNN - Vx = NN - V%01x = %02x\n", x, nn);

}

void opcode7(u16 opcode, CPU *cpu) {

//ADD VX, NN
u8 x = (opcode & 0x0F00) >> 8;
u8 nn = opcode & 0x00FF;

    printf("7XNN - ADD VX, NN - %02x += %02x\n", x, nn);

}

void opcode8(u16 opcode, CPU *cpu) {

//Various register ops depending on last flag
u8 x = (opcode & 0x0F00) >> 8;
u8 y = (opcode & 0x00F0) >> 4;
//u8 nn = opcode & 0x00FF;
u8 o_flag = opcode & 0x000F;

    printf("8XYN = VX VY operation - %02x %02x flag: %01x\n", x, y, o_flag);

    // Extract instruction from last bit
    switch (o_flag) {

    case 0:

      printf("8XY0 - VX = VY - %02x %02x\n", x, y);
      break;

    case 1:

      printf("OR VX, VY = VX |= VY - %02x %02x\n", x, y);
      break;

    case 2:

      printf("AND VX, VY = VX &= VY - %02x %02x\n", x, y);
      break;

    case 3:

      printf("XOR VX, VY = VX ^= VY - %02x %02x\n", x, y);
      break;

    case 4:

      printf("ADD VX, VY += VY - %02x %02x (vx > 0xff ? v[0xf] = 1: 0\n", x, y);

      break;

    case 5:

      printf("SUB VX, VY = VX -= VY - %02x %02x (vx > vy ? 1 : 0)\n", x, y);
      break;

    case 6:

      // SHR VX (VY) - VY ignored in most implementations
      printf("SHR VX - VX >> 1 - %02x /2 - v[0xf] set to VX & 0x01\n", x);
      break;

    case 7:

      // Reverse of 5xy5 in order
      printf("SUBN VX, VY - VY - VX - %02x %02x (v[0xf] = vy < vx ? 1 : 0)\n",
             y, x);
      break;

    case 8:

      printf("SHL VX (VY) - VX <<= 1 - %02x *2\n - v[0xf] = VX & 0x01\n", x);
      break;

    default:

    printf("Unimplemented op code or data?\n");
    break;

    }


}


OpcodeFunction opcodeTable[16] = {
  [0] = opcode0,
  [1] = opcode1,
 [2] = opcode2,
 [3] = opcode3,
 [4] = opcode4,
 [5] = opcode5,
 [6] = opcode6,
 [7] = opcode7,
 [8] = opcode8,

  [9 ... 15] = NULL
};

void get_opcode(CPU *cpu, u8 *buffer) {

  u8 msB = buffer[0];
  u8 lsB = buffer[1];

  u16 opcode = msB << 8 | lsB;
  //  //  u8 reg = target & 0xFF00;
  //  //  u8 nn = target & 0x00FF;

  u8 x = (opcode & 0x0F00);
  u8 y = (opcode & 0x00F0);
  u8 nn = (opcode & 0x00FF);

  u8 o_flag = opcode & 0x000F;

  u8 opcodePrefix = (opcode & 0xF000) >> 12;
  // printf("Reg x is %02x. Reg y is %02x\n", x, y);

  printf("Mem: $%04x - OP code: %04x \n", cpu->registers.PC, opcode);

  OpcodeFunction parseOpcode = opcodeTable[opcodePrefix];

  if (parseOpcode != NULL) {
  parseOpcode(opcode, cpu);
  }

  return;

  switch (opcode & 0xF000) {

  case 0x0000:

    switch (opcode & 0x00FF) {

    case 0x00e0:

      printf("CLS - disp_clear()\n");
      break;

    case 0x00ee:

      printf("RET - return from subroutine\n");
      break;

    default:
      break;
    }
    break;
  
  case 0x1000:

    printf("1NNN - Jump/goto mem address %03x\n", opcode & 0x0FFF);
    break;

  case 0x2000:

    printf("2NNN - Calls function/sub at $%03x\n", opcode & 0x0FFF);

    break;

  // 3XNN - If true PC += 2
  case 0x3000:

    printf("3XNN - if (Vx == NN) %x, %02x - skip if true\n", x, nn);

    break;

  // 4XNN - If NE then PC +=2
  case 0x4000:

    printf("4XNN - if (Vx != NN) %x, %02x - skip if true\n", x, nn);

    break;

  // 5XY0 Compare registers x and y = pc+= 2 if true
  case 0x5000:

    printf("5XY0 - if reg x == y - %02x, %02x\n", x, y);

    break;
  // VXNN - MOV VX, NN
  case 0x6000:

    printf("6XNN - Vx = NN - %02x = %02x\n", x, nn);

    break;

  case 0x7000:

    printf("7XNN - ADD VX, NN - %02x += %02x\n", x, nn);

    break;

  case 0x8000:

    printf("8XYN = VX VY operation - %02x %02x flag: %01x\n", x, y, o_flag);

    // Extract instruction from last bit
    switch (o_flag) {

    case 0:

      printf("8XY0 - VX = VY - %02x %02x\n", x, y);
      break;

    case 1:

      printf("OR VX, VY = VX |= VY - %02x %02x\n", x, y);
      break;

    case 2:

      printf("AND VX, VY = VX &= VY - %02x %02x\n", x, y);
      break;

    case 3:

      printf("XOR VX, VY = VX ^= VY - %02x %02x\n", x, y);
      break;

    case 4:

      printf("ADD VX, VY += VY - %02x %02x (vx > 0xff ? v[0xf] = 1: 0\n", x, y);

      break;

    case 5:

      printf("SUB VX, VY = VX -= VY - %02x %02x (vx > vy ? 1 : 0)\n", x, y);
      break;

    case 6:

      // SHR VX (VY) - VY ignored in most implementations
      printf("SHR VX - VX >> 1 - %02x /2 - v[0xf] set to VX & 0x01\n", x);
      break;

    case 7:

      // Reverse of 5xy5 in order
      printf("SUBN VX, VY - VY - VX - %02x %02x (v[0xf] = vy < vx ? 1 : 0)\n",
             y, x);
      break;

    case 8:

      printf("SHL VX (VY) - VX <<= 1 - %02x *2\n - v[0xf] = VX & 0x01\n", x);
      break;
    }

  case 0x9000:

    printf("9XY0 - SNE X, Y - vx !== vy ? pc+=2\n");

    break;

  case 0xA000:
    printf("ANNN - MEM - I = $%03x\n", opcode & 0x0FFF);
    break;

  case 0xB000:
    printf("BNNN - JMP - PC = NNN + v0\n");
    break;
  case 0xC000:
    printf("CXNN - Vx = rand() & NN\n");
    break;
  case 0xD000:
    printf("DXYN - draw(vx, vy, N) - 8w * nh sprite at x, y coord\n");
    break;

  case 0xE000:

    switch (opcode & 0x0FFF) {

      switch (opcode & 0x00FF) {

      case 0x009e:
        printf("EX9E - SE if key() == vx\n");
        break;

      case 0x00a1:
        printf("EXA1 - SNE if key() != vx\n");
        break;
      }
    }

  case 0xF000:

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
      printf("Unmatched op code %04x. Might be data or sprite data\n", opcode);
    } // end switch (nn)
  }   // end switch (opcode & 0xf000)
} 
