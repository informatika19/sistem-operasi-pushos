#include "headers/mkdir.h"
#include "../../lib/lib.h"

int main () {
  char cwdIdx;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[SECTOR_ENTRY_LENGTH * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0, success;

  getParameter(&cwdIdx, cwdName, argv, &success);

  if (!success) {
    printString("An error occured while reading file ");
    printString(path);
  } else {
    // clear(buf, SECTOR_ENTRY_LENGTH * SECTOR_SIZE); // too big
    strncpy(path, argv[1], MAXIMUM_CMD_LEN);
    printString("giibbe\r\n"); //x
    createFolder(argv[1], cwdIdx);
    printString("\r\nrish\r\n"); //x
    printNumber(res); //x
    printString("\r\n"); //x

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
