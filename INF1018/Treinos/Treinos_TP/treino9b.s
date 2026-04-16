/*
struct S
{
    int v;
    struct S *prox;
};

extern int norma[];

int boo(struct S *s, int n)
{
    int acum = 0;
    while (s)
    {
        s->v = acerta(norma, n+acum);
        acum += n;
        s = s->prox;
    }
    return acum;
}
*/

.extern norma

.globl boo
boo:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)

    movl $0, %ebx /* acum = 0 */
    movq %rdi, %r12

FOR:
    cmpq $0, %r12
    je FIM

    movl %r13d, %edi /* n */
    addl %ebx, %esi /* n + acum */

    movq $norma, %rdi

    call acerta
    movl %eax, (%r12) /* s->v = acerta(norma, n + acum) */
    
    addl %r13d, %ebx /* acum += n */

    movq 8(%r12), %r12 /* s = s->prox */

    jmp FOR

FIM:
    movl %ebx, %eax /* return acum */

    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13

    addq $32, %rsp

    leave
    ret