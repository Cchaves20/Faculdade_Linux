/*
struct Estrutura
{
    int n;
    short *p;
    short d;
    short s;
};

int indice(short a, shrot b);

short foo(struct Estrutura *e)
{
    short res = 0
    for (int i = 0; i < e->n; i++)
    {
        if (indice(e->d, e->s) == i)
        {
            res = e->s;
            break;
        }
    }
    return res;
}
*/

.text
.globl foo
foo:
    pushq %rbp
    movq  %rsp, %rbp
    subq  $32, %rsp
    movq  %rbx, -8(%rbp)
    movq  %r12, -16(%rbp)
    movq  %r13, -24(%rbp)

    movq  %rdi, %r13             /* r13 = e (preservado no call) */
    movl  $0, %r12d              /* res = 0 */
    movl  $0, %ebx               /* i = 0 */

FOR:
    cmpl  (%r13), %ebx           /* i < e->n ? */
    jge   FIM

    movw 16(%r13), %di        /* 1º arg: e->d */
    movw 18(%r13), %si        /* 2º arg: e->s */
    call  indice

    cmpl  %ebx, %eax             /* indice(e->d, e->s) == i ? */
    je    IF

    addl  $1, %ebx               /* i++ */
    jmp   FOR

IF:
    movw  18(%r13), %r12w        /* res = e->s */

FIM:
    movw %r12w, %ax           /* return res */

    movq  -8(%rbp), %rbx
    movq  -16(%rbp), %r12
    movq  -24(%rbp), %r13
    leave
    ret