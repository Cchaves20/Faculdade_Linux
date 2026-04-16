#include<stdio.h>

int is_little();

int main (void)
{
  int tpMemoria = is_little();
  if (tpMemoria == 1)
    {
      printf("Sua memoria eh little-endian!");
    }
  else
  {
    printf("Sua memoria eh big-endian!");
  }
  return 0;
}

int is_little()
{
  unsigned int i = 258;
  unsigned char* p = (char*)&i;
  if 
}

void dump (void *p, int n) {
  unsigned char *p1 = p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}
