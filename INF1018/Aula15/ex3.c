#include <stdio.h>
#include <math.h>
#include "ex3.h"

#define _USE_MATH_DEFINES

int main (void)
{
  float n2;
  n2 = foo2(M_PI, M_PI);
  printf("%f\n", n2);
  return 0;
}
