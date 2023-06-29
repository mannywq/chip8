#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#include <cpu.h>
#include <opcodes.h>
#include <string.h>

#define REG_COUNT 16
#define MEM_SIZE 4096


// end get_opcode
//  // printf("Op code is %02x and target is %03x\n", opcode, target);

CPU cpu;

u8 memory[MEM_SIZE];
// 16 8-bit registers
u8 v[REG_COUNT];

int main(int argc, char **argv) {
/*
  FILE *fp = fopen(argv[1], "rb");

  if (fp == NULL) {
    printf("Error: Couldn't open file %s\n", argv[1]);
  }

  fseek(fp, 0L, SEEK_END);
  cpu.rom_size = ftell(fp);
  rewind(fp); 

  cpu.registers.PC = 0x200;
  
  u8 temp_mem[MEM_SIZE];

  u8 bytes = fread(temp_mem, sizeof(u8), cpu.rom_size, fp);

  if (bytes != cpu.rom_size) {

    printf("Loaded %d bytes\n", bytes);

    printf("Something went wrong loading memory\n");
  }
  printf("Loaded %d bytes\n", bytes);

  printf("File size is %d bytes\n", cpu.rom_size);

  memcpy(cpu.mem + cpu.registers.PC, temp_mem, cpu.rom_size);

  u16 max_offset = 0x200 + cpu.rom_size; 

  */

// Init CPU with filename from argv[1]
  init_cpu(&cpu, argv[1]);


while(cpu.registers.PC < 0x200 + cpu.rom_size) {

    u8 buffer[2];

    //Copy 2 bytes from memory index of PC to buffer
    memcpy(buffer, &cpu.mem[cpu.registers.PC], 2);

    get_opcode(&cpu, buffer);
    cpu.registers.PC += 2;
    //pc_addr += 2;
  }   


  return EXIT_SUCCESS;
}
