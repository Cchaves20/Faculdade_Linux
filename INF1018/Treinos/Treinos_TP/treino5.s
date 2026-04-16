/*
int boba(float v[], float lim, int n) 
{
    int i;
    int a = 0;
    for(i = 0; i < n; i++) 
    {
        if(v[i] > lim) a += 1;
    }
    return a;
} 
*/

.globl boba
boba:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)

    movq $0, %rbx /* i = 0 */
    movl $0, %r12d /* a = 0 */

FOR:
    cmpq %rsi, %rbx /* n > i */
    jge FIM

    movq %rdi, %r13
    movss (%r13, %rbx, 8), %xmm2
    ucomiss %xmm1, %xmm2 /* lim > v[i] */
    jge IF
    
    jmp SKIP

IF:
    addl $1, %r12d /* a++ */
    jmp SKIP

SKIP:
    addq $1, %rbx /* i++ */
    jmp FOR

FIM:
    movl %r12d, %eax /* return a */

    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13
    addq $32, %rsp

    leave
    ret