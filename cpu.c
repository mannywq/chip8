#include <cpu.h>
#include <stdio.h>
#include <string.h>

void init_cpu(CPU *cpu, char *file)
{


  FILE *fp = fopen(file, "rb");

  if (!fp)  {

    printf("Couldn't open file %s\n", file);
  }

  fseek(fp, 0, SEEK_END);

  cpu->rom_size = ftell(fp);

  rewind(fp);

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
printf("$%04x: ", cpu->registers.PC);

//Set initial program counter offset
 cpu->registers.PC = 0x200;

 cpu->registers.SP = cpu->stack[STACK_SIZE -1];

  u8 bytes = fread(cpu->mem + 0x200, sizeof(u8), cpu->rom_size, fp);

 if (bytes != cpu->rom_size) {

    //printf("Loaded %d bytes\n", bytes);

    printf("Something went wrong loading memory\n");
  }

  printf("Loaded %d bytes\n", bytes);

  printf("File size is %d bytes\n", cpu->rom_size);

  cpu->rom_name = file;

  printf("Loaded Rom: %s\n", cpu->rom_name); 

  fclose(fp);

}

void push(CPU *cpu) {

  if (cpu->registers.SP > 0) {

    cpu->registers.SP--;
    cpu->stack[cpu->registers.SP] = cpu->registers.PC;
  }
  else {

    printf("Stack overflow!\n");
}

}

void pop(CPU *cpu) {

  cpu->registers.PC = cpu->stack[cpu->registers.SP];

  if (cpu->registers.SP < STACK_SIZE -1) {

    cpu->registers.SP++;
  }

}







