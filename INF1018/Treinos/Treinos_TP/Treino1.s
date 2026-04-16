/*
Lab 7 ex2:
*/

/*
char S2[] = {65, 108, 111, 32, 123, 103, 97, 108, 101, 114, 97, 125, 33, 0};
int main (void) {
  char *pc = S2;
  while (*pc)
    if (*pc != 123 || *pc != 125)
        printf ("%c", *pc++);
  printf("\n");
  return 0;
}
*/

.data
S2:    .byte 65, 108, 111, 32, 123, 103, 97, 108, 101, 114, 97, 125, 33, 0
Sf:  .string "%c"    /* primeira string de formato para printf */
Sf2: .string "\n"    /* segunda string de formato para printf */

.text
.globl main
main:
      pushq %rbp
      movq %rsp, %rbp
      subq $16, %rsp
      movq %rbx, -8(%rbp)
      movq %r12, -16(%rbp)
      
      movq $S2, %r12
      
L1:
      cmpb  $0, (%r12)  /* if (*pc == 0) ? */
      je  L3          /* goto L3 */

      
      movsbl (%r12), %eax /* eax = *r12 (estendendo o byte para 32 bits */
      
      cmpl $123, %eax /*if(*r12 == '{')*/
      jge L2
      
      movq    $Sf, %rdi    /* primeiro parametro (ponteiro)*/
      movl    %eax, %esi   /* segundo parametro  (inteiro) */
      movl  $0, %eax
      call  printf       /* chama a funcao da biblioteca */
      
      addq $1, %r12 /*r12 += 1*/
      jmp L1 /*goto L1*/

L2:
      addq $1, %r12 /*r12 += 1*/
      jmp L1
      
L3:
      movq    $Sf2, %rdi    /* primeiro parametro (ponteiro)*/
      movl  $0, %eax
      call  printf       /* chama a funcao da biblioteca */
      
      movq  $0, %rax  /* rax = 0  (valor de retorno) */
      movq    -16(%rbp), %r12 /* recupera r12 */
      movq    -8(%rbp), %rbx  /* recupera rbx */
      leave
      ret
