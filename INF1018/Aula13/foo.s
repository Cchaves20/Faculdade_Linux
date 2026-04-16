.text
.globl foo
foo:
	pushq %rbp
	movq  %rsp, %rbp
	addl  $1, %edi
	movl  %edi, %eax
	leave
	ret
