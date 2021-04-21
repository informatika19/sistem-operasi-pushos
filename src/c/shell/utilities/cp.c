#include "headers/cp.h"
#include "../../lib/lib.h"

// TODO: cek yang mau di-link file apa dir
int main() {
  char *resourcePath, *destinationPath;
  int success, *cwdIdx;
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
  interrupt(0x21, (*cwdIdx << 8) + 0x04, buf, *resourcePath, &res);
  if (res <= 0) {  // read error
    goto cp_error;
    interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
    return;
  }

  // write file
  interrupt(0x21, (*cwdIdx << 8) + 0x05, buf, *destinationPath, &res);
  if (res <= 0) {  // write errror
    goto cp_error;
    interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
    return;
  }

  interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
  return;

cp_error:
  interrupt(0x21, 0, "Terjadi kesalahan saat membuat symbolic link\r\n", 0, 0);
  return;
}
