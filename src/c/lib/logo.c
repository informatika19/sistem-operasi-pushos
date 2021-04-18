#include "headers/logo.h"

void printLogoGrafik() {
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",0);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",1);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",2);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",3);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",4);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",5);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",6);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",7);
  lineLogo("           /ooo+:-...-://. -:-..     :o/:. `../+oo/.   ..-/+oo+:-...           ",8);
  lineLogo("           /ooo+:-...-://. -:-..     :o/:. `../+oo/.   ..-/+oo+:-...           ",9);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",10);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",11);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",12);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",13);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",14);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",15);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",16);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",17);
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
