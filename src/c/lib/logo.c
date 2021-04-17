#include "headers/logo.h"

void printLogoGrafik() {
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",0);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",1);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",2);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",3);
  lineLogo("           /ooo+:-...-://. -:-..     :o/:. `../+oo/.   ..-/+oo+:-...           ",4);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",5);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",6);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",7);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",8);
}

void printLogoASCII() {
  printString("                     _____ _____ _____ _____    _____ _____ \r\n");
  printString("                    |  _  |  |  |   __|  |  |  |     |   __|\r\n");
  printString("                    |   __|  |  |__   |     |  |  |  |__   |\r\n");
  printString("                    |__|  |_____|_____|__|__|  |_____|_____|\r\n");
}

void lineLogo(char *string, int y) {
  int x = 0;
  while (string[x] != '\0') {
    x++;
    switch (string[x]) {
      case 'o':
        interrupt(0x10, 0x0C00 + YELLOW, 0, x, y);
        break;
      case '.':
        interrupt(0x10, 0x0C00 + BLUE, 0, x, y);
        break;
      default:
        interrupt(0x10, 0x0C00 + WHITE, 0, x, y);
    }
  }
}
