#include "math.h"

int div(int a, int b) {
  int x = 0;
  while (a >= b) {
    a -= b;
    x++;
  }
  return x;
}

int mod(int a, int b) {
  while (a >= b) {
    a -= b;
  }
  return a;
}
