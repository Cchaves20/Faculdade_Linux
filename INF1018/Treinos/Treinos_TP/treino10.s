/*
int corta(int v[], int i);
long int boo (int *a, int n) {
    long int acc = 0;
    while (n--)
    {
        acc += corta(a, n);
        a++;
    }
    return acc;
}
*/

.globl boo
boo:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)

    movq $0, %rbx /* acc = 0 */
    movl %esi, %r12d
    movq %rdi, %r13
FOR:
    cmpl %r12d, $0
    je FIM

    call corta
    addq %eax, %rbx
    addq $4 %r13
    
    jmp SKIP

SKIP:
    subl $1, %r12d
    jmp FOR

FIM:
    movq %rbx, %rax

    movq -8(%rbp), %rbx
    movq -16(rbp), %r12
    movq -24(%rbp), %r13
    addq $32, %rsp
    
    leave
    ret