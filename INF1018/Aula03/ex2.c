#include <stdio.h>

int odd_ones(unsigned int x) {
  int sum = 0;
  for (; x!=0;x=x>>4) {
    if ((x & 0xf) == 1) {
      sum++;
    }
  }
  return (sum &1);
}
int main() {
  printf("%x tem numero %s de bits\n",0x01010101,odd_ones(0x01010101) ? "impar":"par");
  printf("%x tem numero %s de bits\n",0x01030101,odd_ones(0x01030101) ? "impar":"par");
  return 0;
}
