#include "headers/cp.h"
#include "../../lib/lib.h"

// TODO: cek yang mau di-link file apa dir
int main() {
  char resourcePath[MAXIMUM_CMD_LEN], destinationPath[MAXIMUM_CMD_LEN];
  int success, cwdIdx;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN];
  char buf[16 * SECTOR_SIZE], dir[2 * SECTOR_SIZE];
  char cwdName[FILE_NAME_LENGTH];
  int res = 0;

  getParameter(&cwdIdx, cwdName, argv, &success);
  strncpy(&resourcePath, &argv[1], MAXIMUM_CMD_LEN);
  strncpy(&destinationPath, &argv[2], MAXIMUM_CMD_LEN);

  interrupt(0x21, 0x0002, dir, ROOT_SECTOR, 0);  // readSector
  interrupt(0x21, 0x0002, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);

  // read file
  readFile(buf, resourcePath, &res, cwdIdx);
  if (res <= 0) {  // read error
    goto cp_error;
    exec("shell", 0x3000, &success, 0x00);
    return;
  }

  // write file
  writeFile(buf, destinationPath, &res, cwdIdx);
  if (res <= 0) {  // write errror
    goto cp_error;
    exec("shell", 0x3000, &success, 0x00);
    return;
  }

  exec("shell", 0x3000, &success, 0x00);
  return;

cp_error:
  printString("An error occured while making a copy\r\n");
  return;
}
