#include "headers/string.h"
#include "headers/math.h"

#define WHITE 0xF

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
  char *start = dest;
  while(*src != '\0') {
    *dest = *src;
    dest++;
    src++;
  }
  *dest = '\0';
  return start;
}

char *strncpy(char *dest, char *src, int n) {
  int i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for ( ; i < n; i++)
    dest[i] = '\0';
  return dest;
}

char *strcat(char *dest, char *src) {
  strcpy (dest + strlen (dest), src);
  return dest;
}

char *strncat(char *dest, char *src, int n) {
  int dest_len = strlen(dest);
  int i;

  for (i = 0 ; i < n && src[i] != '\0' ; i++)
    dest[dest_len + i] = src[i];
  dest[dest_len + i] = '\0';

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
    if (key == 0xD) {              
      string[i] = 0x0;
      return;
    } else if (key == 0x8) {
      if (i > 0) {
        string[i] = 0x0;
        i--;
        interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
        i++;
        interrupt(0x10, 0x0E00 + 0x0, 0, 0, 0);
        i--;
        interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
      }
    } else {
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
    interrupt(0x10, 0x0E00 + c, 0x0000 + WHITE, 0x0000, 0x0000);
  } else {
    printNumber(div(number, 10));
    printNumber(mod(number, 10));
  }
}
