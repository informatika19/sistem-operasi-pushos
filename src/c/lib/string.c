#include "string.h"

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
