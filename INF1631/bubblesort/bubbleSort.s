.text
.globl bubbleSort
bubbleSort:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)
    movq %r14, -32(%rbp)
    movq %r15, -40(%rbp)

    movq %rdi, %r15     /* A = callee saved */
    
    movl %esi, -48(%rbp)  /* n --> frame */

    movl %esi, %edx
    movq %rdi, %rsi
    movl $0, %edi
    call imprime
    
    movl $1, %ebx       /* i = 1 */

    movl -48(%rbp), %r12d
    subl $1, %r12d      /* n - 1 */

FOR1:
    cmpl %r12d, %ebx    /* i <= n - 1*/
    jg FIM

    movl %r12d, %r14d
    subl %ebx, %r14d    /* n - 1 - i */

    movl $0, %r13d      /* j = 0 */

FOR2:
    cmpl %r14d, %r13d
    jg FIMFOR1

    movl  (%r15, %r13, 4), %r10d   /* A[j] */
    movl 4(%r15, %r13, 4), %r11d   /* A[j + 1] */

    cmpl %r11d, %r10d   /* A[j] > A[j + 1] */
    jg IF

    addl $1, %r13d       /* j++ */
    jmp FOR2

FIMFOR1:
    movl -48(%rbp), %edx
    movq %r15, %rsi
    movl %ebx, %edi
    call imprime

    addl $1, %ebx       /* i++ */
    jmp FOR1

IF:
    movl %r11d,  (%r15, %r13, 4)   /* A[j] = A[j + 1] */
    movl %r10d, 4(%r15, %r13, 4)   /* A[j + 1] = A[j] */

    addl $1, %r13d       /* j++ */
    jmp FOR2

FIM:
    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13
    movq -32(%rbp), %r14
    movq -40(%rbp), %r15
    leave
    ret