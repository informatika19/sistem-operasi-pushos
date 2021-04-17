#include "headers/ln.h"
#include "../../lib/headers/string.h"
#include "../../lib/headers/fileIO.h"

// TODO: cek yang mau di-link file apa dir
void shell_hardLink(char cwdIdx, char *resourcePath, char *destinationPath) {
  char dir[2 * SECTOR_SIZE];

  int testDI, testRI, i = 0, jmlParents = 0;
  char destinationIndex, resourceIndex;
  char fname[14];
  char parents[64][14];
  int tmp = 2 * SECTOR_SIZE;

  // read sector
  interrupt(0x21, 0x0002, dir, 0x101, 0);
  interrupt(0x21, 0x0002, dir + 512, 0x102, 0);
  testDI = getFileIndex(destinationPath, cwdIdx, dir);
  testRI = getFileIndex(resourcePath, cwdIdx, dir);
  destinationIndex = testDI & 0xFF;
  resourceIndex = testRI & 0xFF;

  if (testDI == -1 && testRI != -1) {
    jmlParents = parsePath1(destinationPath, parents, fname);
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
      printString("sektor penuh\n");
      goto hardLink_error;
      return;
    }

    *(dir + i) = cwdIdx;
    *(dir + i + 1) = *(dir + resourceIndex * 0x10 + 1);
    strncpy(dir + i + 2, fname, 14);

    interrupt(0x21, 0x0003, dir, 0x101, 0);  // writeSector
    interrupt(0x21, 0x0003, dir + 512, 0x102, 0);

    return;
  } else {
    goto hardLink_error;
    return;
  }

hardLink_error:
  interrupt(0x21, 0, "Terjadi kesalahan saat membuat symbolic link\n", 0, 0);
  return;
}
