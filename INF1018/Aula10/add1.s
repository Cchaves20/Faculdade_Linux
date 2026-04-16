.globl add

add:
  pushq %rbp
  movq %rsp, %rbp

  mov $0, %r10

l1: 
  cmpq $0, %rdi
  je end
  
  addq (%rdi), %r10

  movq 8(%rdi), %rdi
  jmp l1

end:
  movq %r10, %rax
  leave
  ret
