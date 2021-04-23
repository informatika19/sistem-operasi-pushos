#include "headers/cat.h"
#include "../../lib/lib.h"

int main() {
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char buf[SECTOR_ENTRY_LENGTH * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0;

  char temp[14];
  char dir[SECTOR_SIZE * 2];
  char sec[SECTOR_SIZE];
  int entry, i;
  char secFlag;
  char secStart;
  char mapDex;
  char sectorBuff[SECTOR_ENTRY_LENGTH];

  char* printbuff[SECTOR_SIZE];

  getParameter(&cwdIdx, cwdName, argv, &success);

  readSector(dir, ROOT_SECTOR);
  readSector(dir + SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(sec, SECTORS_SECTOR);

  // find entry with same name
  entry = 0;
  for (; entry < 2 * SECTOR_SIZE; entry += 0x10) {
    strncpy(temp, dir + entry + 2, FILE_NAME_LENGTH);
    if (strcmp(argv[1], temp) == 0) {
      secFlag = *(dir + entry + 1);
      break;
    } 
  }

  if (entry == 2 * SECTOR_SIZE) {
    printString("File not found!\r\n");
    printString("\r\n");
    exec("shell", 0x3000, &success, 0x00);
  }

  // get list of sectors in map
  secStart = (secFlag * 0x10);
  strncpy(sectorBuff, sec + secStart, SECTOR_ENTRY_LENGTH);

  if (sec == 0xFF) {
    printString("Not a file!\r\n");
    printString("\r\n");
    exec("shell", 0x3000, &success, 0x00);
  }
  
  i = 0;
  for(; i < SECTOR_ENTRY_LENGTH; i++) {
    if (sectorBuff[i] != 0x00) {
      readSector(printbuff, sectorBuff[i]);
      printString(printbuff);
    }    
  }

  printString("\n");
  printString("\n");
  exec("shell", 0x3000, &success, 0x00);
}
