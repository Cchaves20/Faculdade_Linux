/*
int calcula(float f);
double boba (float val) 
{
    int i;
    int pos;
    double d[5];
    for (i=0; i<5; i++) 
    {
        pos = calcula(val);
        d[pos] += val;
    }
    return d[0];
}
*/

.globl boba
boba:
    pushq %rbp
    movq %rsp, %rbp
    subq $56, %rsp
    movq %rbx, -40(%rbp)
    movq %r12, -48(%rbp) 

    movl $0, %ebx /* i = 0*/
    movq $0, %r12 /* pos = 0 */

FOR:
    cmpl $5, %ebx /* 5 > i */
    jge FIM

    call calcula
    movq %rax, %r12 /* pos = calcula(val) */

    cvtss2sd %xmm0, %xmm1 /* double val */
    movsd -40(%rbp, %r12, 8), %xmm2 /* d[pos] += val */
    addsd %xmm1, %xmm2
    movsd %xmm2, -40(%rbp, %r12, 8)

    jmp SKIP

SKIP:
    add $1, %ebx /* i++ */
    jmp FOR

FIM:
    movsd -40(%rbp), %xmm0

    movq -40(%rbp), %rbx
    movq -48(%rbp), %r12
    addq $56, %rsp

    leave
    ret