/*
int acerta (int u[], int i)
{
    return u[i];
}
*/

.globl acerta
acerta:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp

    movl (%rdi, %rsi, 4), %eax

    leave
    ret