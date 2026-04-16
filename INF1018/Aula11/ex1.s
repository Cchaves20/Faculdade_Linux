/*
int novonum(void) {
  int minhalocal;
  printf("numero: ");
  scanf("%d",&minhalocal);
  return minhalocal;
}
*/

.data
S1: .string "%d"
S2: .string "numero: "

.text
.globl novonum
novonum:
	
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rbp
	
	movq $S2, %rdi
	call printf
	
	movq $S1, %rdi
	leaq -4(%rbp), %rsi
	call scanf
	
	movq -4(%rbp), %rax
        
	leave
	ret
