#include "headers/cat.h"
#include "../../lib/headers/fileIO.h"

void shell_cat(char cwdIdx, char *path) {
  char buf[16 * SECTOR_SIZE];
  int res = 0;

  interrupt(0x21, (cwdIdx << 8) + 0x04, buf, path, &res);

  if (res > 0)
    interrupt(0x21, 0, buf, 0, 0);
  else {
    interrupt(0x21, 0, "Terjadi kesalahan saat membaca berkas ", 0, 0);
    interrupt(0x21, 0, path, 0, 0);
  }
  interrupt(0x21, 0, "\n", 0, 0);
}
