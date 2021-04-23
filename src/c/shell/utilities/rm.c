#include "headers/rm.h"
#include "../../lib/lib.h"

// need to protecc bin
int main () {
  char cwdIdx;
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[16 * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  int res = 0;

  readSector(map, MAP_SECTOR);
  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);
  readSector(sec, SECTORS_SECTOR);

  getParameter(&cwdIdx, cwdName, argv, &success);

  if (!success) {
    printString("An error occured while reading file ");
    printString(path);
    printString("\r\n");    
  }

  else{
    int idx = getFileIndex(argv[1], cwdIdx, dir);
    if (dir + idx + 1 == '\xFF'){
      deleteFolder(argv[1], cwdIdx);
    }
    else{
      int res;
      removeFile(argv[1], &res ,cwdIdx);
    }
  }

  exec("shell", 0x3000, &success, 0x00);
}
