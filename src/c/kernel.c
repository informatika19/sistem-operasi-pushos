#include "kernel.h"
#include "io.h"
#include "filesystem.h"

int main() {
  char buffer[512];
  char *success;

  makeInterrupt21();

  executeProgram("logo", 0x3000, &success, 0x00); // segmennya dukun anjay

  while (1);
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
    for (i = 0; i < 512 * 16; i++) {
      putInMemory(segment, i, fileBuffer[i]);
    }
    launchProgram(segment);
  } else {
    interrupt(0x21, 0, "File not found!", 0,0);
  }
}
