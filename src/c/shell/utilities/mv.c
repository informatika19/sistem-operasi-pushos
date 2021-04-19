#include "headers/mv.h"
#include "../../lib/headers/fileIO.h"

int main() {
  int *success;
  interrupt(0x21, 0x0006, "shell", 0x3000, &success, 0);
}
