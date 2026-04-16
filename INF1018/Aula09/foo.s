globl foo

/*void foo(int a[]@<rdi>, int n@<rsi>)*/

foo:

  pushq %rbp 
  movq  %rsp, %rbp
  subq  $16, %rsp
  movq  %r12, -8(%rbp)
  movq  %rbx, -16(%rbp)

  movq %rdi, %r12
  movl $0, %ebx
  movq $0, %r11
l1:
  cmpl %esi, %ebx
  jg ln

  /*calcular a[1]*/
  movl  %ebx, %ecx
  imull $4, %ecx
  movq  %r12, %r10

  addq  %rcx, %r10
  addq  (%r10), %r11

  cmp   $0, (%r10)
  jne l3
  movq  %r11, (%r10)
  movq  $0, %r11

l3:
  incl %ebx
  jmp l1


ln:
  movq -8(%rbp), %r12
  movq -16(%rbp), %rbx
  leave
  ret
