/*
double trata(int u) 
{
    return (double) u;
}
*/

.globl trata
trata:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp

    cvtsi2sd %edi, %xmm0 /* (double) u */

    leave
    ret