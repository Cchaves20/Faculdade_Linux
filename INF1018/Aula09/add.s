.text

.globl add

add:
  /*int add(int a@<rdi>, int b@<rsi>, int c@<rdx>)*/
  push %rbp
  mov %rsp, %rbp

  movq %rdi, %rax
  addq %rsi, %rax
  addq %rdx, %rax

  leave
  ret
