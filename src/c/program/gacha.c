#include "gacha.h"

#include "../lib/headers/string.h"
#include "../lib/headers/math.h"
#include "../lib/headers/fileIO.h"

int main() {
  int success, num, res;
  char str[4];

  printString("\nWelcome to pushOS's\n");
  printString("G A C H A\n\n");
  printString("Input number [0..100]: ");

  readString(str);
  num = str2int(str);

  res = mod(div(num, 7), 2);

  printString("\nYour input: ");
  printNumber(num);
  printString("\nand you got ");

  if (res = 1) {
    printString("Zhongli\n");
  } else {
    printString("a Sacrificial Bow\n");
  }
  
  printString("Thanks for playing!");

  printString("\n");
  printString("\n");
  exec("shell", 0x3000, &success, 0x00);
}
