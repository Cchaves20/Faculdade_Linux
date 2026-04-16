/*
int bemboba (int num) {
  int local[4];
  int *a;
  int i;

  for (i=0,a=local;i<4;i++) {
    *a = num;
    a++;
  }
  return addl (local, 4);
}
*/

.text
.globl bemboba
bemboba:
        pushq %rbp
        movq %rsp, %rbp
        subq $16, %rbp
        
        movq $0, %r11
        leaq -16(%rbp), %r10
        
FOR:
        cmpq $4, %r11
        je END
        
        movq %rdi, (%r10)
        
        incq %r11
        
        addq $4, %r10
        jmp FOR
        
END:
        leaq -16(%rbp), %rdi
        movl $4, %esi
        call addl
        
        leave
        ret
