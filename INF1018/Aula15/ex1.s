/*
float foo (double a, float b) {
  return (a+b)*(a-b);
}
*/

.text
.globl foo
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movq %rbx, -8(%rsp)
	movq %r12, -16(%rsp)
	
	movl $0, %ebx
	movl $0, %r12d
	cvtsi2sd %ebx, %xmm2
	cvtsi2sd %r12d, %xmm3
	
	cvtss2sd %xmm1, %xmm1
	addsd %xmm1, %xmm2
	addsd %xmm0, %xmm2
	subsd %xmm1, %xmm3
	subsd %xmm0, %xmm3
	mulsd %xmm2, %xmm3
	cvtsd2ss %xmm3, %xmm3
	movss %xmm3, %xmm0
	
	movq -8(%rsp), %rbx
	movq -16(%rsp), %r12
	leave
	ret
