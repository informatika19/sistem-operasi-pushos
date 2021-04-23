#include "headers/ln.h"
#include "../../lib/lib.h"

// TODO: cek yang mau di-link file apa dir
int main() { 
  // getParameter
  int success;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdIdx, cwdName[FILE_NAME_LENGTH];

  char resourcePath[MAXIMUM_CMD_LEN], destinationPath[MAXIMUM_CMD_LEN];
  char dir[2 * SECTOR_SIZE];
  int testDI, testRI, i = 0, jmlParents = 0;
  char destinationIndex, resourceIndex;
  char fname[FILE_NAME_LENGTH];
  char parents[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH];
  int tmp = 2 * SECTOR_SIZE;

  getParameter(&cwdIdx, cwdName, argv, &success);
  strncpy(resourcePath, argv[1], MAXIMUM_CMD_LEN);
  strncpy(destinationPath, argv[2], MAXIMUM_CMD_LEN);

  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);

  testDI = getFileIndex(destinationPath, cwdIdx, dir);
  testRI = getFileIndex(resourcePath, cwdIdx, dir);

  destinationIndex = testDI & 0xFF;
  resourceIndex = testRI & 0xFF;

  if (testDI == -1 && testRI != -1) {
    jmlParents = parsePath(destinationPath, parents, fname);
    if (jmlParents != 0) {
      clear(destinationPath, strlen(destinationPath));
      strncpy(destinationPath, parents[0], strlen(parents[0]));
      strncat(destinationPath, "/", 14);

      for (i = 1; i < jmlParents; ++i) {
        strncat(destinationPath, parents[i], strlen(parents[i]));
        strncat(destinationPath, "/", 2);
      }

      cwdIdx = getFileIndex(destinationPath, cwdIdx, dir) & 0xFF;
    }

    i = 0;
    while (*(dir + i + 2) != 0 && i < tmp) {
      i += 0x10;
    }
    
    if (*(dir + i + 2) != 0) {  // sektor files penuh
      printString("Sectors are full\r\n");
      goto hardLink_error;
      exec("shell", 0x3000, &success, 0x00);
      return;
    }

    *(dir + i) = cwdIdx;
    *(dir + i + 1) = *(dir + resourceIndex * 0x10 + 1);
    strncpy(dir + i + 2, fname, 14);

    writeSector(dir, ROOT_SECTOR);
    writeSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);

    printString("\r\n");
    exec("shell", 0x3000, &success, 0x00);
    return;
  } else {
    goto hardLink_error;
    exec("shell", 0x3000, &success, 0x00);
    return;
  }

hardLink_error:
  printString("An error occured while making symbolic link\r\n");
  printString("\r\n");
  return;
}
