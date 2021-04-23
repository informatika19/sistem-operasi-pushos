#include "headers/cat.h"
#include "../../lib/lib.h"

int main() {
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char buf[SECTOR_ENTRY_LENGTH * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0;

  getParameter(&cwdIdx, cwdName, argv, &success);

  strncpy(path, argv[1], MAXIMUM_CMD_LEN);
  if (!success) {
    printString("An error occured while reading file ");
    printString(path);
  } else {
    // clear(buf, SECTOR_ENTRY_LENGTH * SECTOR_SIZE); // too big
    readFile(buf, path, &res, cwdIdx); // buggy

    if (res > 0) {
      printString(buf);
    } else {
      printString("An error occured while reading file ");
      printString(path);
    }
  }

  printString("\r\n");
  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
}
