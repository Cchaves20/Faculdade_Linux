/* 
void foo (double *vd, int n) 
{
    while (n--)
    {
        *vd = 0.0;
        vd++;
    }
}
*/

.globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq %rbx, -8(%rbp)

    movl %esi, %ebx

FOR:
    cmpl $0, %ebx
    je FIM

    movq $0, (%rdi)

    addq $8, %rdi

    jmp SKIP

SKIP:
    sub $1, %ebx
    jmp FOR

FIM:
    movq -8(%rbp), %rbx
    
    leave
    ret