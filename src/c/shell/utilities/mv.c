#include "headers/mv.h"
#include "../../lib/lib.h"

int main() {
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char src[MAXIMUM_CMD_LEN], dest[MAXIMUM_CMD_LEN], dir[2 * SECTOR_SIZE];
  char srcFName[FILE_NAME_LENGTH], destFName[FILE_NAME_LENGTH];
  char parentIndexSrc, parentIndexDest;

  getParameter(&cwdIdx, cwdName, argv, &success);

  strncpy(src, argv[1], MAXIMUM_CMD_LEN);
  strncpy(dest, argv[2], MAXIMUM_CMD_LEN);

  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_SIZE, ROOT_SECTOR+1);

  parentIndexSrc = getFileIndex(src, cwdIdx, dir);
  parentIndexDest = getFileIndex(dest, cwdIdx, dir);

  if (parentIndexSrc < 0 || parentIndexDest < 0) {
    printString("Invalid path\r\n");
    goto exec_shell;
    return;
  }

  printNumber(parentIndexSrc);
  printString("\r\n");
  printNumber(parentIndexDest);
  printString("\r\n");

  if (*(dir + parentIndexDest * FILE_ENTRY_LENGTH + 1) != 0xFF) {
    printString("Destination is a file\r\n");
    goto exec_shell;
    return;
  }

  *(dir + parentIndexSrc * FILE_ENTRY_LENGTH) = parentIndexDest;

  writeSector(dir, ROOT_SECTOR);
  writeSector(dir+SECTOR_SIZE, ROOT_SECTOR+1);

exec_shell:
  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
  return;
}
