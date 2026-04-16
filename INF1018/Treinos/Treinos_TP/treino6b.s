/*
struct exam { float val; int tipo; };

double inv (double val);

float boo (double d, float bias) 
{
    int i;
    struct exam temp[5];
    for (i=0; i<5; i++) 
    {
        temp[i].val = inv(d);
        temp[i].tipo = i;
    }  
    return temp[0].val + bias;
}
*/

.globl boo
boo:
    pushq %rbp
    movq %rsp, %rbp

    subq $40, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)

    movl $0, %ebx /* i em ebx*/
    
    lea -40(%rbp), %r13

FOR:
    cmpl $5, %ebx /* compara i < 5 */
    jge FIM

    call inv
    cvtsd2ss %xmm0, %xmm0 /* converte inv(d) em int */
    movss %xmm0, (%r13) /*temp[i].val = inv(d) */
    
    movl %ebx, 4(%r13) /*temp[i].tipo = i */
    addq $8, %r13 /* add 4 e, rbp */
    jmp SKIP

SKIP:
    addl $1, %ebx /* i++ */
    jmp FOR

FIM:
    lea -40(%rbp), %r13 /* volta ao temp[0] */
    movss (%r13), %xmm0 /* carrega temp[0].val */
    addss %xmm1, %xmm0 /* soma temp[0].val + bias */

    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13
    addq $40, %rsp
    
    leave
    ret