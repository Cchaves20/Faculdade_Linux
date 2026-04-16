/*
double diff(double a, double b, int n)
{
    double s[2] = {0.0, 0.0};
    while (n--)
    {
        s[0] += a;
        s[1] += b;
    }
    return (s[0] - s[1]);
}
*/

.globl diff
diff:
    pushq %rbp
    movq %rsp, %rbp

    subq $32, %rsp
    movq %rbx, -24(%rbp)

    movq $0, -8(%rbp)
    movq $0, -16(%rbp)
    movl %edi, %ebx

FOR:
    cmpl $0, %ebx
    je FIM

    cvttsd2siq %xmm0, %r10
    addq %r10, -8(%rbp)
    cvttsd2siq %xmm1, %r11
    addq %r11, -16(%rbp)

    jmp SKIP

SKIP:
    subl $1, %ebx
    jmp FOR

FIM:
    cvtsi2sd -8(%rbp), %xmm1
    cvtsi2sd -16(%rbp), %xmm0
    subsd %xmm1, %xmm0

    movq -24(%rbp), %rbx

    leave
    ret