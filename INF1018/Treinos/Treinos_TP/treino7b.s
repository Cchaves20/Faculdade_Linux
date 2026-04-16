/*
int calculaIndice(int valor);
double foo (int v[], int n) 
{
    double m = 1.0;
    int i;
    int indice;
    for(i=0; i< n; i++) 
    {
        indice = calculaIndice(i);
        m *= trata(v[indice]);
    }
    return m;
}
*/

.globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)

    movq %rdi, %rbx
    movl %esi, %r13d

    movl $1, %r10d
    cvtsi2sd %r10d, %xmm1 /* m */
    xorl %ebx, %ebx /* i */
    movl $0, %r12d /* indice */

FOR:
    cmpl %r13d, %ebx /* n > i */
    jge FIM

    movl %ebx, %edi
    call calculaIndice /* indice = calculaIndice(i) */

    movl (%rbx, %rax, 4), %edi /* movendo v[indice] para rax */
    call trata
    mulsd %xmm0, %xmm1 /* m *= trata */

    jmp SKIP

SKIP:
    add $1, %ebx /* i++ */
    jmp FOR

FIM:
    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13
    addq $24, %rsp
    
    movsd %xmm1, %xmm0
    
    leave
    ret