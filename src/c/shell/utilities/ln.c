#include "headers/ln.h"
#include "../../lib/lib.h"

// TODO: cek yang mau di-link file apa dir
int main() { 
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char dir[SECTOR_SIZE * 2];
  int entry, i;
  char secFlag;

  char temp[14];  
  char test[14];

  getParameter(&cwdIdx, cwdName, argv, &success);
  
  readSector(dir, ROOT_SECTOR);
  readSector(dir + SECTOR_SIZE, ROOT_SECTOR+1);

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


  if (!success) {
    printString("An error occured");
  } else {
    createFolder(cwdIdx, argv[2], secFlag);
  }

  printString("\r\n");
  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
}
