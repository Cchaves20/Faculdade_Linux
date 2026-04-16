 .data
  nums: .int 3, -5, 7, 8, -2
  s1:   .string "%d\n"
.text
.globl main
main:
/* prologo */
  pushq %rbp
  movq  %rsp, %rbp
  subq  $16, %rsp
  movq  %rbx, -8(%rbp)
  movq  %r12, -16(%rbp)

  movl $0, %ebx
  movq $nums, %r12
  /* coloque seu codigo aqui */
l1:
  cmp $5, %ebx
  je l2

  movq (%r12), %rdi
  movq $1, %rsi
  call filtro

  movq $s1, %rdi
  movl %eax, %esi
  call printf
  
  incl %ebx
  addq $4, %r12
  jmp l1
l2:
/* finalizacao */
  movq -8(%rbp), %rbx
  movq -16(%rbp), %r12
  leave
  ret
