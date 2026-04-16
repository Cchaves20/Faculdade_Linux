/*
char S2[] = {65, 108, 111, 32, 123, 103, 97, 108, 101, 114, 97, 125, 33, 0};
int main (void) {
  char *pc = S2;
  while (*pc)
    printf ("%c", *pc++);
  printf("\n");
  return 0;
}
*/

.data

  Sf:  .string "%d\n"

.text
.globl main
main :
/**********************************************************/
/* mantenha este trecho aqui - prologo !!! */
  pushq % rbp
  movq % rsp , % rbp
  subq $16 , % rsp
  movq % rbx , -8(% rbp )
  movq % r12 , -16(% rbp )
/**********************************************************/

  mov $1, %ebx

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

/**********************************************************/
/* mantenha este trecho aqui - finalizacao !!!! */
  movq $0 , % rax 
  movq -8(% rbp ) , % rbx
  movq -16(% rbp ) , % r12
  leave
  ret
/**********************************************************/
