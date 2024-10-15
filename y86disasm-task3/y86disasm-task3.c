#include <stdio.h>
#include <stdlib.h>

// This is an array of register mnemonics in y86
const char *register_names[] = {"%eax", "%ecx", "%edx",
                                "%ebx", "%esp", "%ebp",
                                "%esi", "%edi", "UNKNOWN_REGSITER"};

// This is an array of instruction mnemonics in y86
const char *opcode_names[27] = {
    "halt",   "nop",    "ret",    "rrmovl", "cmovle", "cmovl", "cmove",
    "cmovne", "cmovge", "cmovg",  "addl",   "subl",   "andl",  "xorl",
    "pushl",  "popl",   "irmovl", "rmmovl", "mrmovl", "jmp",   "jle",
    "jl",     "je",     "jne",    "jge",    "jg",     "call"};

// This is an array of instructions in y86, in hexadecimal
const int opcode_bytes[27] = {
    0x00, 0x10, 0x90, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x60, 0x61, 0x62, 0x63, 0xA0, 0xB0, 0x30, 0x40,
    0x50, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x80,
};

int findIndex(const int *array_to_search, int target, int size);

// This is the help function that reads y86 binary code from a file
unsigned int loadBinFromFile(const char *filename, unsigned char memory[],
                             unsigned int memsize);

int main(int argc, char **argv) {
  unsigned int MAX_MEMSIZE = 4096;
  unsigned char
      memory[MAX_MEMSIZE]; // This array represents the 4KB memory space
  unsigned int PC = 0;     // This is the initial program counter address

  if (argc < 2) {
    printf(
        "Usage: please specify a y86 binary program file in the argument.\n");
    return -1;
  }

  unsigned int program_size = loadBinFromFile(argv[1], memory, MAX_MEMSIZE);

  if (program_size == 0)
    return 0;

  // TODO: Implement your disassembler from here
  // At the moment, it simply display each byte from the memory.

  while (PC < program_size) {

    int opcode_index = 0;
    // Find the Index of the opcode in the array of opcodes, stored in hex
    opcode_index = findIndex(opcode_bytes, memory[PC], 27);

    // If the returened opcode index is -1 (not in the array)
    // Then return an error message
    if (opcode_index == -1) {
      printf("TODO: undisassembled opcode. The first byte of the instruction "
             "is 0x%X\n",
             memory[PC]);

      // If the returned opcode index is between 0 and 2
      // Print the opcode name
    } else if (opcode_index >= 0 && opcode_index <= 2) {

      printf("%s\n", opcode_names[opcode_index]);

      // If the returned opcode index is between 3 and 13
      // Find the correct registers in the register_names array
      // Print the opcode name, followed by the register names
    } else if (opcode_index >= 3 && opcode_index <= 13) {
      int regA = (memory[PC + 1] & 0xf0) >> 4;
      int regB = memory[PC + 1] & 0x0f;
      PC += 1;

      printf("%s %s, %s\n", opcode_names[opcode_index], register_names[regA],
             register_names[regB]);

      // If the returned opcode index is between 14 and 15
      // Find the correct register in the register_names array
      // Print the opcode name, followed by the register name
    } else if (opcode_index >= 14 && opcode_index <= 15) {
      int regA = (memory[PC + 1] & 0xf0) >> 4;
      PC += 1;

      printf("%s %s\n", opcode_names[opcode_index], register_names[regA]);

      // If the returned opcode index is 16
      // Find the correct register in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the register name, followed by the
      // intermediate value
    } else if (opcode_index == 16) {
      PC += 1;
      int regB = memory[PC] & 0x0f;

      PC += 1;
      signed int operand = (memory[PC] | memory[PC + 1] << 8 |
                            memory[PC + 2] << 16 | memory[PC + 3] << 24);
      PC += 3;

      printf("%s $%d, %s\n", opcode_names[opcode_index], operand,
             register_names[regB]);

      // If the returned opcode index is 17
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the first register name, followed by
      // the intermediate value followed by the last register name
    } else if (opcode_index == 17) {
      PC += 1;
      int regA = (memory[PC] & 0xf0) >> 4;
      int regB = memory[PC] & 0x0f;

      PC += 1;

      signed int operand = (memory[PC] | memory[PC + 1] << 8 |
                            memory[PC + 2] << 16 | memory[PC + 3] << 24);

      PC += 3;

      printf("%s %s, %d(%s)\n", opcode_names[opcode_index],
             register_names[regA], operand, register_names[regB]);

      // If the returned opcode index is 18
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the intermediate value
      // followed by the last register name, followed by the first register
      // name,
    } else if (opcode_index == 18) {
      PC += 1;
      int regA = (memory[PC] & 0xf0) >> 4;
      int regB = memory[PC] & 0x0f;
      PC += 1;
      signed int operand =
          (memory[PC] | memory[PC + 1] | memory[PC + 2] | memory[PC + 3]);
      PC += 3;

      printf("%s %d(%s), %s\n", opcode_names[opcode_index], operand,
             register_names[regB], register_names[regA]);

      // If the returned opcode index is between 19 and 26
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the intermediate value
    } else if (opcode_index >= 19 && opcode_index <= 26) {
      PC += 1;
      signed int operand =
          (memory[PC] | memory[PC + 1] | memory[PC + 2] | memory[PC + 3]);
      PC += 3;

      printf("%s %d\n", opcode_names[opcode_index], operand);
    }

    PC++;
  }

  return 0;
}

/****************************************************************************
N.B. You do not need to modify or work in this function.
Description:
This function reads in a y86 machine bytecode from file and
store them in an unsigned char array.
******************************************************************************/
unsigned int loadBinFromFile(const char *filename, unsigned char memory[],
                             unsigned int memsize) {
  unsigned int bytes_read = 0;

  unsigned int file_size = 0;

  FILE *pfile = fopen(filename, "rb");

  if (!pfile) {
    printf("Unable to load file %s, please check if the path and name are "
           "correct.\n",
           filename);
    return 0;
  }

  fseek(pfile, 0, SEEK_END);
  file_size = ftell(pfile);
  rewind(pfile);

  if (file_size > memsize) {
    printf("Program size exceeds memory size of %d.\n", memsize);
    return 0;
  }

  bytes_read = fread(memory, 1, file_size, pfile);

  if (bytes_read != file_size) {
    printf("Bytes read does not match the file size.\n");
    return 0;
  }

  fclose(pfile);

  return bytes_read;
}

// Function to loop through an array to search for a value
// Returns the index of the matching value
// Or -1 if the value is not found
int findIndex(const int *array_to_search, int target, int size) {
  // Initialise the counter i as 0
  int i = 0;

  // While the counter i is less than the size of the array
  while (i < size) {
    // Return i if the value at index i
    // is the same as the target value
    if (array_to_search[i] == target) {
      return i;
    } else {
      // Incrememnt counter i
      i++;
    }
  }
  // Return -1 if the value is not found
  return -1;
}
