#include <cpu.h>
#include <stdio.h>

void init_cpu(CPU *cpu, char *file)
{


  FILE *fp = fopen(file, "rb");

  if (!fp)  {

    printf("Couldn't open file %s\n", file);
  }

  fseek(fp, 0, SEEK_END);

  cpu->rom_size = ftell(fp);

  rewind(fp);

//Set initial program counter offset
 cpu->registers.PC = 0x200;

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











