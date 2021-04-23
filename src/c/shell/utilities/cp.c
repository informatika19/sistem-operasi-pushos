#include "headers/cp.h"
#include "../../lib/lib.h"

int main() {
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char resourcePath[MAXIMUM_CMD_LEN], destinationPath[MAXIMUM_CMD_LEN];
  char buf[16 * SECTOR_SIZE];
  int res = 0;

  getParameter(&cwdIdx, cwdName, argv, &success);
  strncpy(resourcePath, argv[1], MAXIMUM_CMD_LEN);
  strncpy(destinationPath, argv[2], MAXIMUM_CMD_LEN);

  readFile(buf, resourcePath, &res, cwdIdx);
  if (res < 0) {  // read error
    goto cp_error;
    exec("shell", 0x3000, &success, 0x00);
    return;
  }

  // write file
  writeFile(buf, destinationPath, &res, cwdIdx);
  if (res < 0) {  // write errror
    goto cp_error;
    exec("shell", 0x3000, &success, 0x00);
    return;
  }

  printString("\r\n");
  exec("shell", 0x3000, &success, 0x00);
  return;

cp_error:
  printString("An error occured while making a copy\r\n");
  printString("\r\n");
  return;
}
