#include <stdio.h>

unsigned char switch_byte(unsigned char x) {
  return (x & 0xf0)>>4 | (x & 0x0f) << 4;
}

unsigned char rotate_left(unsigned char x, int n) {
  return (x<<n) | (x>>(8-n));
}

int main(void) {
  printf("%x\n", switch_byte(0xAB));
  printf("----------------\n");
  printf("%x\n", rotate_left(0x61,1));
  printf("%x\n", rotate_left(0x61,2));
  printf("%x\n", rotate_left(0x61,7));
  return 0;
}
