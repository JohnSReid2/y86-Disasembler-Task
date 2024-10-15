#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int convertStrToByteCode(const char *str, unsigned char inst[], int size);

int findIndex(const int *array_to_search, int target, int size);

int main(int argc, char **argv) {
  FILE *pFile = NULL;

  char buffer[15];

  if (argc < 2) {
    pFile = fopen("./test2.txt", "r");
  } else {
    pFile = fopen(argv[1], "r");
  }

  if (pFile == NULL) {
    printf("Error open test file, please make sure they exist.\n");

    return 0;
  }

  while (fgets(buffer, 15, pFile) && strlen(buffer) > 1) {
    // This unsigned char array stores an instruction read from the file
    // As the largest y86 instruction is 6 bytes, there are 6 unsigned char in
    // the array where each represents a byte.
    unsigned char instruction[6] = {0, 0, 0, 0, 0, 0};
    convertStrToByteCode(buffer, instruction, 6);

    // TODO: From here, your task is to complete the implementation so that all
    // y86 opcode and operands can be disassembled. Any undisassembled opcode
    // should display as "TODO: undisassembled opcode and operands"
    int opcode_index = 0;
    // Find the Index of the opcode in the array of opcodes, stored in hex
    opcode_index = findIndex(opcode_bytes, instruction[0], 27);

    // If the returened opcode index is -1 (not in the array)
    // Then return an error message
    if (opcode_index == -1) {
      printf("TODO: undisassembled opcode. The first byte of the instruction "
             "is 0x%X\n",
             instruction[0]);

      // If the returned opcode index is between 0 and 2
      // Print the opcode name
    } else if (opcode_index >= 0 && opcode_index <= 2) {

      printf("%s\n", opcode_names[opcode_index]);

      // If the returned opcode index is between 3 and 13
      // Find the correct registers in the register_names array
      // Print the opcode name, followed by the register names
    } else if (opcode_index >= 3 && opcode_index <= 13) {
      int regA = (instruction[1] & 0xf0) >> 4;
      int regB = instruction[1] & 0x0f;

      printf("%s %s, %s\n", opcode_names[opcode_index], register_names[regA],
             register_names[regB]);

      // If the returned opcode index is between 14 and 15
      // Find the correct register in the register_names array
      // Print the opcode name, followed by the register name
    } else if (opcode_index >= 14 && opcode_index <= 15) {
      int regA = (instruction[1] & 0xf0) >> 4;

      printf("%s %s\n", opcode_names[opcode_index], register_names[regA]);

      // If the returned opcode index is 16
      // Find the correct register in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the register name, followed by the
      // intermediate value
    } else if (opcode_index == 16) {
      int regB = instruction[1] & 0x0f;
      signed int operand = (instruction[2] | instruction[3] << 8 |
                            instruction[4] << 16 | instruction[5] << 24);

      printf("%s $%d, %s\n", opcode_names[opcode_index], operand,
             register_names[regB]);

      // If the returned opcode index is 17
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the first register name, followed by
      // the intermediate value followed by the last register name
    } else if (opcode_index == 17) {
      int regA = (instruction[1] & 0xf0) >> 4;
      int regB = instruction[1] & 0x0f;
      signed int operand = (instruction[2] | instruction[3] << 8 |
                            instruction[4] << 16 | instruction[5] << 24);

      printf("%s %s, %d(%s)\n", opcode_names[opcode_index],
             register_names[regA], operand, register_names[regB]);

      // If the returned opcode index is 18
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the intermediate value
      // followed by the last register name, followed by the first register
      // name,
    } else if (opcode_index == 18) {
      int regA = (instruction[1] & 0xf0) >> 4;
      int regB = instruction[1] & 0x0f;
      signed int operand = (instruction[2] | instruction[3] << 8 |
                            instruction[4] << 16 | instruction[5] << 24);

      printf("%s %d(%s), %s\n", opcode_names[opcode_index], operand,
             register_names[regB], register_names[regA]);

      // If the returned opcode index is between 19 and 26
      // Find the correct registers in the register_names array
      // Convert the intermediate value
      // Print the opcode name, followed by the intermediate value
    } else if (opcode_index >= 19 && opcode_index <= 26) {
      signed int operand = (instruction[1] | instruction[2] << 8 |
                            instruction[3] << 16 | instruction[4] << 24);

      printf("%s %d\n", opcode_names[opcode_index], operand);
    }
  }

  fclose(pFile);

  return 0;
}

/****************************************************************************
N.B. You do not need to modify or work in this function.
Description:
This function converts a line of machine code read from the text file
into machine byte code.
The machine code is stored in an unsigned char array.
******************************************************************************/
int convertStrToByteCode(const char *str, unsigned char inst[], int size) {
  int numHexDigits = 0;
  char *endstr;
  // Each instruction should consist of at most 12 hex digits
  numHexDigits = strlen(str) - 1;
  // Convert the string to integer, N.B. this integer is in decimal
  long long value = strtol(str, &endstr, 16);

  int numBytes = numHexDigits >> 1;
  int byteCount = numHexDigits >> 1;

  while (byteCount > 0) {
    unsigned long long mask = 0xFF;
    unsigned long shift = (numBytes - byteCount) << 3;

    inst[byteCount - 1] = (value & (mask << shift)) >> shift;
    byteCount--;
  }

  // Return the size of the instruction in bytes
  return numHexDigits >> 1;
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
