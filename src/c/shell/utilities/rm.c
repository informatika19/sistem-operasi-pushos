#include "headers/rm.h"
#include "../../lib/lib.h"

// need to protecc bin
int main () {
  char cwdIdx;
  int success, idx;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[16 * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  int res = 0;

  getParameter(&cwdIdx, cwdName, argv, &success);
  strncpy(&path, &argv[1], MAXIMUM_CMD_LEN);

  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);

  idx = getFileIndex(argv[1], cwdIdx, dir);

  if (idx != -1) {
    if (*(dir + 16*idx + 1) == 0xFF) {
      deleteFolder(argv[1], cwdIdx);
    } else {
      removeFile(argv[1], &res ,cwdIdx);
      printString("Successfully removed file ");
      printString(argv[1]);
      printString("\r\n");
      printString("\r\n");
    }
    exec("shell", 0x3000, &success, 0x00);
  }
  else{
    printString("File/Folder not found!\r\n");
    printString("\r\n");
    exec("shell", 0x3000, &success, 0x00);
  }
}
