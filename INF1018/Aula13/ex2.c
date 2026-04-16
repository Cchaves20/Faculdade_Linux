#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_START(P) ((intptr_t)(P)&~(pagesize-1))
#define PAGE_END(P) (((intptr_t)(P)+pagesize-1)&~(pagesize-1))

/*
 * The execpage() function shall change the specified memory pages
 * permissions into executable.
 *
 * void *ptr  = pointer to start of memory buff
 * size_t len = memory buff size in bytes
 *
 * The function returns 0 if successful and -1 if any error is encountered.
 * errono may be used to diagnose the error.
 */
int execpage(void *ptr, size_t len) {
	int ret;

	const long pagesize = sysconf(_SC_PAGE_SIZE);
	if (pagesize == -1)
		return -1;

	ret = mprotect((void *)PAGE_START(ptr),
		 PAGE_END((intptr_t)ptr + len) - PAGE_START(ptr),
		 PROT_READ | PROT_WRITE | PROT_EXEC);
	if (ret == -1)
		return -1;

	return 0;
}

#undef PAGE_START
#undef PAGE_END
/*
 0000000000000000 <foo>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   e8 00 00 00 00          call   9 <foo+0x9>
   9:   c9                      leave
   a:   c3                      ret
 */
unsigned char shellcode[] ={0x55,
                            0x48,0x89,0xe5,
                            0xe8,0x12,0xd2,0xff,0xff,
                            0xc9,
                            0xc3};
typedef int (*funcp) (int x);

int add (int x) {
  return x+1;
}

int main(void) {
  execpage(shellcode, sizeof(shellcode));
  funcp f;
  f = (funcp)shellcode;
  int i;
  i = (*f)(10);
  printf("%d\n",i);
  return 0;
}
