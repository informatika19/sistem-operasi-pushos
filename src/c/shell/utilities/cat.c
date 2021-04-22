#include "headers/cat.h"
#include "../../lib/lib.h"

int main() {
  int cwdIdx, success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[16 * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0;

  printString("MASUK CAT\r\n"); //x
  getParameter(&cwdIdx, cwdName, argv, &success);
  printNumber(cwdIdx); //x

  printString(" <<< cwdIdx\r\n"); //x
  if (success == 1) printString("get param cat success\r\n"); //x

  if (!success) {
    printString("An error occured while reading file ");
    printString(path);
  } else {
    strncpy(&path, &argv[1], MAXIMUM_CMD_LEN);
    readFile(buf, path, &res, cwdIdx);

    if (res > 0) {
      printString(buf);
    } else {
      printString("An error occured while reading file ");
      printString(path);
    }
  }

  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
}
