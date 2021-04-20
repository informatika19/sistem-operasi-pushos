#include "headers/cat.h"
#include "../../lib/headers/fileIO.h"

int main() {
  int *cwdIdx, *success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN];
  char buf[16 * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  int res = 0;
  getParameter(&cwdIdx, argv);
  if (argv[1] == 0xFF) {
    interrupt(0x21, 0, "An error occured while reading file ", 0, 0);
    interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
  }

  strncpy(&path, &argv[1], MAXIMUM_CMD_LEN);
  interrupt(0x21, (*cwdIdx << 8) + 0x04, buf, path, &res);

  if (res > 0) {
    interrupt(0x21, 0, buf, 0, 0);
  } else {
    interrupt(0x21, 0, "An error occured while reading file ", 0, 0);
    interrupt(0x21, 0, path, 0, 0);
  }
  interrupt(0x21, 0, "\r\n", 0, 0);
  interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
}
