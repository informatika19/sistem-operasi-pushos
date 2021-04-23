#include "headers/mkdir.h"
#include "../../lib/lib.h"

int main () {
    char cwdIdx;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[SECTOR_ENTRY_LENGTH * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0, success;

  getParameter(&cwdIdx, cwdName, argv, &success);

  if (!success) {
    printString("An error occured");
  } else {
    createFolder(cwdIdx, argv[1], 0xFF);
  }

  // printString("huhi\r\n");
  exec("shell", 0x3000, &success, 0x00);
}
