#include <stdio.h>

void foo(int a[], int n);

int main(void) {
  int a[4] = {1,2,0,0};

  foo(a, 4);

  for(int i =0; i<4; i++) {
    printf("%d\n", a[i]);
  }
  return 0;
}
