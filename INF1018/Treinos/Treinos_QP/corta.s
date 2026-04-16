/*
int corta(char c1, int b)
{
    return c1 + b;
}
*/

.text
.globl corta
corta:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)

    movb %dil, %r12b
    movl %esi, %ebx
    addl %r12b, %ebx
    movl %ebx, %eax

    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12

    leave
    ret