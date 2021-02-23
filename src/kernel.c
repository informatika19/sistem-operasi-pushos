void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); // Fungsi untuk mengisi buffer dengan 0

int main() {
  char tes[16];
  interrupt(0x10, 0x0003, 0, 0);
  makeInterrupt21();
  readString(tes);
  // tes[0]='A';
  // tes[1]='B';
  // tes[2]='C';
  // tes[3]='\0';
  printString(tes);
  clear(tes,16);
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
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
    interrupt(0x10, 0xe * 256 + *string, 0, 0, 0);
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

void clear(char *buffer, int length){
  int i;
  for(i=0;i<length;i++){
    buffer[i]=0;
  }
}
