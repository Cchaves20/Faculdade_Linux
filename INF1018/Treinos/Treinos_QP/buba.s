/*
int buba(char *v, int n, char c)
{
    int i, s = 0;
    for (i=0; i<n; i++)
    {
        if (v[i] <= c)
            s += corta(v[i], n);
    }
    return s;
}
*/

.text
.globl buba
buba:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)

    movl $0, %ebx
    movl $0, %r12d

FOR:
    cmpl %ebx %esi
    jg FIM

    cmpb (%edi, %ebx, 1) %dl
    jg SKIP

IF
    movb (%edi, %ebx, 1), %dil
    call corta
    addl %eax, %r12d

SKIP:
    addl $1, %ebx
    jmp FOR

FIM
    movl %r12d, %eax

    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12

    leave
    ret