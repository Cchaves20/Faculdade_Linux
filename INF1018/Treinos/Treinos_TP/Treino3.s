/*  
#include <stdio.h>

int nums[4] = {65, -105, 111, 34};

int main (void) {
  int i;
  int s = 0;

  for (i=0;i<4;i++)
    s = s+nums[i];

  printf ("soma = %d\n", s);

  return 0;
}
*/

.data

nums: .int 65, -105, 111, 34
Sf: .string "soma = %d\n"

.text
.globl main
main:
      
      pushq %rbp
      movq %rsp, %rbp
      subq $16, %rsp
      movq %rbx, -8(%rbp)
      movq %r12, -8(%rbp)
      
      movl $0, %ebx
      movl $0, %r12d
L1:

      cmpl $4, %r12d
      jge L2
      
      movslq %r12d, %rcx
      imulq $4, %rcx
      addq $nums, %rcx
      addl (%rcx), %ebx
      
      addl $1, %r12d
      jmp L1
      
L2:

      movq $Sf, %rdi
      movl %ebx, %esi
      movl $0, %eax
      call printf
      
      movl $0, %eax
      movq -8(%rbp), %rbx
      movq -16(%rbp), %r12
      leave
      ret
