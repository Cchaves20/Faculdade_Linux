#include <stdio.h>

#define makefloat(s,e,f) ((s & 1)<<31 | (((e) & 0xff) << 23) | ((f) & 0x7fffff))
#define getsig(x)  ((x)>>31 & 1)
#define getexp(x)  ((x)>>23 & 0xff)
#define getfrac(x) ((x) & 0x7fffff)

typedef union { 
  float f;
  unsigned int i;
 } U;

float float2(float f) {
  U out;
  out.f = f;
  out.i = makefloat(getsig(out.i), getexp(out.i)+1, getfrac(out.i));
  return out.f;
}

int main(void) {
  printf("%f\n", float2(15213.0));
  return 0;
}
