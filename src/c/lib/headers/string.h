#ifndef _STRING_H_
#define _STRING_H_

int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, int n);
int strlen(char *s);
char *strcpy(char *dest, char *src);
char *strncpy(char *dest, char *src, int n);
char *strcat(char *dest, char *src);
char *strncat(char *dest, char *src, int n);

void printString(char *string);
void readString(char *string);
void printNumber(int number);

void int2str (char *string, int number);
void str2int (char *string, int *number);
int dec2hex (int number);

#endif
