#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define REG_COUNT 16
#define MEM_SIZE 4096

// index register
uint16_t I;
// Stack
uint8_t stack[64];
// Stack pointer
uint8_t sp;

// 64x32 video buffer
uint8_t video_buffer[32][64];

// program counter
uint16_t pc = 0;

uint8_t v[REG_COUNT];

typedef struct {

  struct {

    uint8_t v[16];
    uint16_t I;
    uint16_t SP;
    uint16_t PC;

  } registers;

  uint8_t display[32][64];
  char *mem[4096];

  uint8_t delay_timer, sound_timer;

  uint8_t halt;

  bool key[16];

  char *rom_name;

} CPU;

void get_op_code(uint16_t buffer) {

  uint16_t op_code = (buffer & 0xF000) >> 12;
  uint16_t target = buffer & 0x0FFF;
  uint8_t reg = target & 0xFF00;
  uint8_t val = target & 0x00FF;

  uint8_t reg_x = (target & 0x0F00) >> 8;
  uint8_t reg_y = (target & 0x00F0) >> 4;
  uint8_t nn = target & 0x00FF;

  uint8_t o_flag = target & 0x000F;

  // printf("Reg x is %02x. Reg y is %02x\n", reg_x, reg_y);

  printf("OP code: %04x\n", buffer);

  switch (buffer) {

  case 0x00e0:

    printf("CLS - disp_clear()\n");
    break;

  case 0x00ee:

    printf("RET - return from subroutine\n");
    break;

  default:
    break;
  }

  switch (op_code) {

  case 0x0000:
    printf("RCA1802 call instruction not implemented\n");

    break;

  case 0x1:

    printf("1NNN - Jump/goto mem address $%03x\n", target);
    break;

  case 0x2:

    printf("2NNN - Calls function/sub at $%03x\n", target);

    break;

  // 3XNN - If true PC += 2
  case 0x3:

    printf("3XNN - if (Vx == NN) - skip if true\n");
    printf("Comparing register $%02x to value %02x\n", reg, val);

    break;

  // 4XNN - If NE then PC +=2
  case 0x4:

    printf("4XNN - If (Vx != NN) = skip if true\n");
    printf("Comparing reg $%02x with val %03x\n", reg, val);

    break;

  // 5XY0 Compare registers x and y = pc+= 2 if true
  case 0x5:

    printf("5XY0 - if reg x == y - %02x, %02x\n", reg_x, reg_y);

    break;
  // VXNN - MOV VX, NN
  case 0x6:

    printf("6XNN - Vx = NN - %02x = %02x\n", reg_x, nn);

    break;

  case 0x7:

    printf("7XNN - ADD VX, NN - %02x += %02x\n", reg_x, nn);

    break;

  case 0x8:

    printf("8XYN = VX VY operation - %02x %02x flag: %01x\n", reg_x, reg_y,
           o_flag);

    // Extract instruction from last bit
    switch (o_flag) {

    case 0:

      printf("8XY0 - VX = VY - %02x %02x\n", reg_x, reg_y);
      break;

    case 1:

      printf("OR VX, VY = VX |= VY - %02x %02x\n", reg_x, reg_y);
      break;

    case 2:

      printf("AND VX, VY = VX &= VY - %02x %02x\n", reg_x, reg_y);
      break;

    case 3:

      printf("XOR VX, VY = VX ^= VY - %02x %02x\n", reg_x, reg_y);
      break;

    case 4:

      printf("ADD VX, VY += VY - %02x %02x (vx > 0xff ? v[0xf] = 1: 0\n", reg_x,
             reg_y);

      break;

    case 5:

      printf("SUB VX, VY = VX -= VY - %02x %02x (vx > vy ? 1 : 0)\n", reg_x,
             reg_y);
      break;
    case 6:

      // SHR VX (VY) - VY ignored in most implementations
      printf("SHR VX - VX >> 1 - %02x /2 - v[0xf] set to VX & 0x01\n", reg_x);
      break;
    case 7:

      // Reverse of 5xy5 in order
      printf("SUBN VX, VY - VY - VX - %02x %02x (v[0xf] = vy < vx ? 1 : 0)\n",
             reg_y, reg_x);
      break;
    case 8:

      printf("SHL VX (VY) - VX <<= 1 - %02x *2\n - v[0xf] = VX & 0x01\n",
             reg_x);
      break;
    }

  case 0x9:

    printf("9XY0 - SNE X, Y - vx !== vy ? pc+=2\n");

    break;

  case 0xA:
    printf("ANNN - MEM - I = NNN\n");
    break;

  case 0xB:
    printf("BNNN - JMP - PC = NNN + v0\n");
    break;
  case 0xC:
    printf("CXNN - Vx = rand() & NN\n");
    break;
  case 0xD:
    printf("DXYN - draw(vx, vy, N) - 8w * nh sprite at x, y coord\n");
    break;
  case 0xE:
    if ((target & 0xFF) == 0x9e) {

      printf("EX9E - SE if key() == vx\n");
      break;

    } else if ((target & 0xff) == 0xa1) {
      printf("EXA1 - SNE if key() != vx\n");
      break;
    }
    break;

  case 0xF:

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
    }
  }
  // printf("Op code is %02x and target is %03x\n", op_code, target);
}

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

  uint16_t *buffer = malloc(f_size);

  // Read full size of file into buffer
  fread(buffer, 2, f_size, fp);

  printf("File size is %d bytes\n", f_size);

  while (pc < f_size) {

    // pc += decode(buffer, pc);
    // printf("Position %04x in buffer\n", pc);
    get_op_code(buffer[pc]);
    pc++;
  }

  fclose(fp);

  return EXIT_SUCCESS;
}
