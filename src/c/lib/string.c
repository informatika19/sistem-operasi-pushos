#include "headers/string.h"
#include "headers/math.h"


int strcmp(char *s1, char *s2) {
  for (; *s1 && *s1 == *s2; s1++, s2++);
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(char *s1, char *s2, int n) {
  for (; *s1 && n && *s1 == *s2; n--, s1++, s2++);
  return (*(unsigned char*)s1 - *(unsigned char*)s2) * (n != 0);
}

int strlen(char* s) {
  int i = 0;
  while(s[i] != '\0') { i++; }
  return i;
}

char *strcpy(char *dest, char *src) {
  int i;
  for (i = 0; src[i] != '\0'; i++) dest[i] = src[i];
  dest[i] = 0;
  return dest;
}

char *strncpy(char *dest, char *src, int n) {
  int i;
  for (i = 0; i < n && src[i] != '\0'; i++) dest[i] = src[i];
  for (; i < n; i++) dest[i] = '\0';
  return dest;
}

char *strcat(char *dest, char *src) {
  int n_dest = strlen(dest), i;
  for (i = 0; src[i] != '\0'; i++) dest[n_dest + i] = src[i];
  dest[n_dest + i] = '\0';
  return dest;
}

char *strncat(char *dest, char *src, int n) {
  int n_dest = strlen(dest), i;
  for (i = 0; i < n && src[i] != '\0'; i++) dest[n_dest + i] = src[i];
  dest[n_dest + i] = '\0';
  return dest;
}

void printString(char *string) {
  while (*string != '\0') {
    interrupt(0x10, 0x0E00 + *string, 0, 0, 0);
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
        printString("\r\n\0");
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

void printNumber(int number) {
  char c;

  if (number < 0) {
    printString("-");
    number *= -1;
  }

  if (number < 10) {
    c = number + '0';
    interrupt(0x10, 0x0E00 + c, 0, 0, 0);
  } else {
    printNumber(div(number, 10));
    printNumber(mod(number, 10));
  }
}

void int2str (char* string, int number) {
  char s[6];

  if (number < 0) {
    strncat(string, "-", 1);
    number *= -1;
  }

  if (number < 10) {
    s[0] = number + '0';
    s[1] = 0;
    strncat(string, s, 1);
  } else {
    int2str(string, div(number, 10));
    int2str(string, mod(number, 10));
  }
}

int str2int (char* string) {
  // TODO: handle negatives
  int i, res;
  char s[6];

  strncpy(s, string, 6);
  res = s[0] && 0;
  for (i = 1; i < 6 && s[i] != 0; i++) {
    res = res * 10 + s[i] && 0;
  }

  return res;
}
