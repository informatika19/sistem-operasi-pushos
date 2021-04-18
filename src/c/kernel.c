#include "kernel.h"

#include "lib/lib.h"
#include "shell/shell.h"

int main() {
  char buffer[SECTOR_SIZE];

  makeInterrupt21();

  interrupt(0x10, 0x0013, 0, 0, 0); // set video mode
  printLogoGrafik();
  interrupt(0x15, 0x8600, 0, 4, 0); // sleep
  interrupt(0x10, 0x0003, 0, 0, 0); // set video mode
  printLogoASCII();
  printString("press enter to continue...");
  readString(0);
  interrupt(0x10, 0x0003, 0, 0, 0);
  shell();

  while (true);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  char AL, AH; 
  AL = (char) (AX); 
  AH = (char) (AX >> 8); 
  switch (AX) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02: 
      readSector(BX, CX); 
      break; 
    case 0x03: 
      writeSector(BX, CX); 
      break; 
    case 0x04: 
      readFile(BX, CX, DX, AH); 
      break; 
    case 0x05: 
      writeFile(BX, CX, DX, AH); 
      break;
    case 0x06:
      executeProgram(BX, CX, DX, AH);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void executeProgram(char *filename, int segment, int *success, char parentIndex) {
  // Buat buffer
  int isSuccess;
  char fileBuffer[512 * 16];
  // Buka file dengan readFile
  readFile(&fileBuffer, filename, &isSuccess, parentIndex);
  // If success, salin dengan putInMemory
  if (isSuccess) {
    // launchProgram
    int i = 0;
    for (i = 0; i < 512*16; i++) {
      putInMemory(segment, i, fileBuffer[i]);
    }
    launchProgram(segment);
  } else {
    interrupt(0x21, 0, "File not found!", 0,0);
  }
}

void clear(char *buffer, int length) {
  int i;
  for (; i < length; i++) {
    buffer[i] = 0;
  }
}
