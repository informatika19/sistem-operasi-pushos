#include "io.h"

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    *(buffer + i) = 0;
  }
}

void printString(char *string) {
  while (*string != '\0') {
    if (*string == '\n') {
      interrupt(0x10, 0x0E00 + '\r', 0, 0, 0);
      interrupt(0x10, 0x0E00 + '\n', 0, 0, 0);
    } else {
      interrupt(0x10, 0x0E00 + *string, 0, 0, 0);
    }
    string++;
  }
}

void readString(char *string) {
  int i = 0;
  while (1) {
    int key = interrupt(0x16, 0, 0, 0, 0);
    switch (key) {
      case 0xD:
        string[i] = 0x0;
        return;
      case 0x8:
        if (i > 0) {
          i--;
          interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
          i++;
          interrupt(0x10, 0x0E00 + 0x0, 0, 0, 0);
          i--;
          interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
        }
        break;
      default:
        string[i] = key;
        interrupt(0x10, 0x0E00 + key, 0, 0, 0);
        i++;
    }
  }
}
