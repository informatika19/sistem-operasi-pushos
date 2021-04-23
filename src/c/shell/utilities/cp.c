#include "headers/cp.h"
#include "../../lib/lib.h"

int main() {
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char resourcePath[MAXIMUM_CMD_LEN], destinationPath[MAXIMUM_CMD_LEN];
  char buf[16 * SECTOR_SIZE];
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
  strncpy(resourcePath, argv[1], MAXIMUM_CMD_LEN);
  strncpy(destinationPath, argv[2], MAXIMUM_CMD_LEN);

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
      strncpy(buf + i * SECTOR_SIZE, printbuff, SECTOR_SIZE);
    }    
  }

  if (entry == 2 * SECTOR_SIZE) {
    printString("File not found!\r\n");
    printString("\r\n");
    exec("shell", 0x3000, &success, 0x00);
  }

  printString("CONTENTS OF COPIED FILE \r\n");
  printString(buf);
  printString("\r\n\n");

  // readFile(buf, resourcePath, &res, cwdIdx);
  // if (res < 0) {  // read error
  //   goto cp_error;
  //   exec("shell", 0x3000, &success, 0x00);
  //   return;
  // }

  // write file
  writeFile(buf, destinationPath, &res, cwdIdx);
  // if (res < 0) {  // write errror
  //   goto cp_error;
  //   exec("shell", 0x3000, &success, 0x00);
  //   return;
  // }

  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
  return;

// cp_error:
//   printString("An error occured while making a copy\r\n");
//   printString("\r\n");
//   return;
}
