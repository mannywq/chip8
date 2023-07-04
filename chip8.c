#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#include <cpu.h>
#include <opcodes.h>
#include <string.h>
#include <SDL.h>

#define REG_COUNT 16
#define MEM_SIZE 4096


// end get_opcode
//  // printf("Op code is %02x and target is %03x\n", opcode, target);

static CPU cpu;

u8 memory[MEM_SIZE];
// 16 8-bit registers
u8 v[REG_COUNT];

int main(int argc, char **argv) {

  init_cpu(&cpu, argv[1]);


while(cpu.registers.PC < 0x200 + cpu.rom_size) {

    u8 buffer[2];

    //Copy 2 bytes from memory index of PC to buffer
    memcpy(buffer, &cpu.mem[cpu.registers.PC], 2);

    //Increment PC before handling OP code to make sure jumps etc work properly
    cpu.registers.PC += 2; 
    get_opcode(&cpu, buffer);
  }   


  return EXIT_SUCCESS;
}
