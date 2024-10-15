#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    pFile = fopen("./test1.txt", "r");
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
    // y86 opcodes can be disassembled. Any undisassembled opcode should display
    // as "TODO: undisassembled opcode"
    int opcode_index = 0;
    // Find the Index of the opcode in the array of opcodes, stored in hex
    opcode_index = findIndex(opcode_bytes, instruction[0], 27);

    // If the returened opcode index is -1 (not in the array)
    // Then return an error message
    if (opcode_index == -1) {
      printf("TODO: undisassembled opcode. The first byte of the instruction "
             "is 0x%X\n",
             instruction[0]);
      // Otherwise print the opcode name
    } else {
      printf("%s\n", opcode_names[opcode_index]);
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
      // Incrememnt i
      i++;
    }
  }
  // Return -1 if the value is not found
  return -1;
}