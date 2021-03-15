#include "string.h"

int strcmp(const char *s1, const char *s2) {
  for (; *s1 && *s1 == *s2; s1++, s2++);
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, int n) {
  for (; *s1 && n && *s1 == *s2; n--, s1++, s2++);
  return (*(unsigned char*)s1 - *(unsigned char*)s2) * (n != 0);
}

int strlen(const char* s) {
  int i = 0;
  while(s[i] != '\0') { i++; }
  return i;
}

char *strcpy(char *dest, const char *src) {
  char *start = dest;
  while(*src != '\0') {
    *dest = *src;
    dest++;
    src++;
  }
  *dest = '\0';
  return start;
}

char *strncpy(char *dest, const char *src, int n) {
  int i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for ( ; i < n; i++)
    dest[i] = '\0';
  return dest;
}
