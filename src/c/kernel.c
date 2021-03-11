#include "lib/kernel.h"
#include "lib/math.h"

#define VIDMEM 0xA000
#define SCREEN_WIDTH 320
#define BLUE 0x9
#define YELLOW 0xE
#define WHITE 0xF

int main() {
  char test[16];
  interrupt(0x10, 0x0013, 0, 0, 0);
  printLogoGrafik();
  interrupt(0x15, 0x8600, 0, 5, 0);
  interrupt(0x10, 0x0003, 0, 0, 0x0C00);
  printLogoASCII();
  while (1) {
    printString("Masukkan input : ");
    readString(test);
    printString(test);
    clear(test,16);
  }
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
  while (*string != '\0') {
    interrupt(0x10, 0x0E00 + *string, 0, 0, 0);
    string++;
  }
}

void readString(char *string) {
  int i = 0;
  while(i < 16) {
    *string = interrupt(0x16, 0, 0, 0, 0);
    
    if (*string != '\r') {
      string++;
      i++;
    } else {
      *string = '\0';
      i = 16;
    }
  }
}

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0;
  }
}

void printLogoGrafik() {
  lineLogo("                                                                               ",0);
  lineLogo("                                                                               ",1);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",2);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",3);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",4);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",5);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",6);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",7);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",8);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",9);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",10);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",11);
  lineLogo("           //ooo+:-...-:////. -:-..     :o/:. `../+oo/.   ..-/+oo+:-           ",12);
  lineLogo("           //ooo+:-...-:////. -:-..     :o/:. `../+oo/.   ..-/+oo+:-           ",13);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",14);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",15);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",16);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",17);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",18);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",19);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",20);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",21);
  lineLogo("           `-:.  .++++++:--.` ``.-:-.`` `` ``.-::-.` ` `--` `.  .--`           ",22);
  lineLogo("           `-:.  .++++++:--.` ``.-:-.`` `` ``.-::-.` ` `--` `.  .--`           ",23);
  lineLogo("                                                                               ",24);
  lineLogo("                                                                               ",25);
}

void printLogoASCII() {
  printString(" _____ _____ _____ _____    _____ _____ ");
  printString("|  _  |  |  |   __|  |  |  |     |   __|");
  printString("|   __|  |  |__   |     |  |  |  |__   |");
  printString("|__|  |_____|_____|__|__|  |_____|_____|");
}
                                                                  
void lineLogo(char * string, int y) {
  int j = 0;
  while (string[j] != '\0') {
    // putInMemory(VIDMEM, (80 * y + j) * 2, string[j]);
    j++;
    switch (string[j]) {
    case 'o':
      putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), YELLOW);
      break;
    case '.':
      putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), BLUE);
      break;
    default:
     putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), WHITE);
    }
  }
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}
