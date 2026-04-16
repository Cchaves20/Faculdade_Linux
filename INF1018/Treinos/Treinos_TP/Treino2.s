/*
int main (void)
{
  int i = 1, j;
  while (i<=10)
  {
    j = i * i;
    printf("%d ", j);
    i++;
  }
}
*/

.data

Sf:  .string "%d\n"

.text
.globl main
main:
      pushq %rbp
      movq %rsp, %rbp
      subq $16 , % rsp
      movq % rbx , -8(% rbp )
      
      movl $1, %ebx
      
L1:
      cmpl $10, %ebx
      jg L2
      
      movl %ebx, %eax
      imull %eax, %eax
      
      movq $Sf, %rdi
      movl %eax, %esi
      movl $0, %eax
      call printf       
      
      addl $1, %ebx
      jmp L1
      
L2:
      movl $0 , % eax 
      movq -8(% rbp ) , % rbx
      leave
      ret
