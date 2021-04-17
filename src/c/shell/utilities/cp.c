#include "headers/cp.h"
#include "../../lib/headers/fileIO.h"

// TODO: cek yang mau di-link file apa dir
void shell_cp(char cwdIdx, char *resourcePath, char *destinationPath) {
  char buf[16 * SECTOR_SIZE];
  char dir[2 * SECTOR_SIZE];
  int res = 0;

  interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
  interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

  // read file
  interrupt(0x21, (cwdIdx << 8) + 0x04, buf, resourcePath, &res);
  if (res <= 0) {  // read error
    goto cp_error;
    return;
  }

  // write file
  interrupt(0x21, (cwdIdx << 8) + 0x05, buf, destinationPath, &res);
  if (res <= 0) {  // write errror
    goto cp_error;
    return;
  }

  return;

cp_error:
  interrupt(0x21, 0, "Terjadi kesalahan saat membuat symbolic link\n", 0, 0);
  return;
}
