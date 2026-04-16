/*
double foo3 (double *a, int n) {
  int i;
  double r = 0.0;
  for (i=0; i<n; i++) {
    r += sin(*a);
    a++;
  }
  return r;
}
*/

.text
.globl foo3
foo3:
        pushq %rbp
        movq  %rsp, %rbp
        subq  $32,  %rsp
        movq %rbx, -8(%rbp)
	movl $0, %ebx
	movq $0, %r10 
        cvtsi2sdq %r10, %xmm1
loop:
	cmpl %esi, %ebx
	jge end

	movsd %xmm1, -16(%rbp)
	movq %rdi, -24(%rbp)
	movl %esi, -28(%rbp)
	movsd (%rdi), %xmm0
	call sin

	movsd -16(%rbp), %xmm1
	movq -24(%rbp), %rdi
	movl -28(%rbp), %esi

	addsd %xmm0, %xmm1
	addq $8, %rdi
	incl %ebx
	jmp loop

end:
	movsd %xmm1, %xmm0
  	movq -8(%rbp), %rbx
	leave
	ret
