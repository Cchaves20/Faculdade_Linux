/*
int fat (int n) {
  if (n==0)
  {
	return 1;
  }
  else 
  {
	temp = n-1;
	temp = n * fat(temp);
	return temp;
  }
}
*/

.globl fat
fat:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rbx, -8(%rbp)
	movq %r12, -16(%rbp)

IF:
	cmpl $0, %edi
	jne ELSE

	movl $1, %eax
	jmp FIM

ELSE:
	movl %edi, %ebx
	subl $1, %edi
	call fat
	movl %eax, %r12d
	imul %edi, %r12d
	movl %r12d, %eax

FIM:

	movq -8(%rbp), %rbx
	movq-16(%rbp), %r12

	leave
	ret