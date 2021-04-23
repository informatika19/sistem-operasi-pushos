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
    if (strcmp(argv[1], temp) == 0)
    {
      secFlag = *(dir + entry + 1);
      break;
    } 
  }

  // get list of sectors in map
  secStart = (secFlag * 0x10);
  strncpy(sectorBuff, sec + secStart, SECTOR_ENTRY_LENGTH);
  
  i = 0;
  for(; i < SECTOR_ENTRY_LENGTH; i++)
  {
    if (sectorBuff[i] != 0x00)
    {
      readSector(printbuff, sectorBuff[i]);
      printString(printbuff);
    }    
  }

  // printString(cwdName);
  // printString("\r\n");
  // printString(argv[1]);

  // readSector(test, 0x58);
  // printString(test);

  // strncpy(path, argv[1], MAXIMUM_CMD_LEN);
  // if (!success) {
  //   printString("An error occured while reading file! ");
  //   printString(path);
  // } else {
  //   // clear(buf, SECTOR_ENTRY_LENGTH * SECTOR_SIZE); // too big
  //   readFile(&buf, path, &res, cwdIdx); // buggy

  //   if (res > 0) {
  //     printString(buf);
  //   } else {
  //     printString(buf);
  //     printString("An error occured while reading file ");
  //     printString(path);
  //   }
  // }

  printString("\r\n");
  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
}
