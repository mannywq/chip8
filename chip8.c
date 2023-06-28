#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>

#define REG_COUNT 16
#define MEM_SIZE 4096

// index register
uint16_t I;
// Stack
u8 stack[64];
// Stack pointer
u8 sp;

// 64x32 video buffer
u8 video_buffer[32][64];

// program counter
uint16_t pc = 0;
uint16_t pc_addr = 0x200;

uint8_t v[REG_COUNT];

typedef struct {

  struct {

    uint8_t v[16];
    uint16_t I;
    uint16_t SP;
    uint16_t PC;

  } registers;

  uint8_t display[32][64];
  uint8_t *mem[4096];

  uint8_t delay_timer, sound_timer;

  uint8_t halt;

  bool key[16];

  char *rom_name;

} CPU;

void get_opcode(uint8_t *buffer) {

  uint8_t msB = buffer[pc];
  uint8_t lsB = buffer[pc + 1];

  uint16_t opcode = msB << 8 | lsB;
  //  //  uint8_t reg = target & 0xFF00;
  //  //  uint8_t nn = target & 0x00FF;

  uint8_t x = (opcode & 0x0F00);
  uint8_t y = (opcode & 0x00F0);
  uint8_t nn = (opcode & 0x00FF);

  uint8_t o_flag = opcode & 0x000F;

  // printf("Reg x is %02x. Reg y is %02x\n", x, y);

  printf("Mem: $%04x - OP code: %04x \n", pc_addr, opcode);

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
} // end get_opcode
//  // printf("Op code is %02x and target is %03x\n", opcode, target);

uint8_t memory[MEM_SIZE];
// 16 8-bit registers
uint8_t v[REG_COUNT];

int main(int argc, char **argv) {

  FILE *fp = fopen(argv[1], "rb");

  if (fp == NULL) {
    printf("Error: Couldn't open file %s\n", argv[1]);
  }

  fseek(fp, 0L, SEEK_END);
  int f_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  // Try starting from 0x200
  //  fseek(fp, 0x200, SEEK_SET);

  uint8_t *buffer = malloc(f_size);

  // Read full size of file into buffer
  fread(buffer, 2, f_size, fp);

  printf("File size is %d bytes\n", f_size);

  while (pc < f_size) {

    // pc += decode(buffer, pc);
    // printf("Position %04x in buffer\n", pc);
    get_opcode(&buffer[pc]);
    pc += 2;
    pc_addr += 2;
  }

  fclose(fp);

  return EXIT_SUCCESS;
}
