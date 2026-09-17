.data
 
SA: .string "A"
SL: .string "L"
SR: .string "R"
Sprintf: .string "\nPivo = %d\n"
SprintfVazia: .string "   "
Sprintfn: .string "\n\n"
 
.text
.globl quicksort
quicksort:
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp
 
    movq %rbx, -8(%rbp)
    movq %r12, -16(%rbp)
    movq %r13, -24(%rbp)
    movq %r14, -32(%rbp)
    movq %r15, -40(%rbp)
 
    movq %rdi, %rbx             /* A */
    movl %esi, %r12d            /* n */
 
    cmpl $1, %r12d              /* caso base */
    jle FIM
 
    movl %esi, %r13d            /* %r13 = n   (movl zera os 32 bits altos) */
    imulq $16, %r13             /* 16n bytes: multiplo de 16 por construcao */
    subq %r13, %rsp             /* abre o bloco na pilha                    */
 
    movl %esi, %r14d            /* %r14 = n        */
    imulq $4, %r14              /* %r14 = 4n       */
 
    movq %rsp, %r13             /* L = base do bloco */
    addq %rsp, %r14             /* R = 4n + L        */
 
    movl $0, -48(%rbp)          /* nL = 0 */
    movl $0, -52(%rbp)          /* nR = 0 */
    movl (%rbx), %r15d          /* p  = A[0] */
    movl $1, -56(%rbp)          /* i  = 1 */
 
FOR1:
    movl -56(%rbp), %ecx        /* i */
    cmpl %r12d, %ecx            /* i < n */
    jge POSFOR1
 
    movl (%rbx,%rcx,4), %r11d   /* A[i] */
    cmpl %r15d, %r11d           /* A[i] <= p */
    jle IF
 
    movl -52(%rbp), %ecx        /* nR */
    movl %r11d, (%r14,%rcx,4)   /* R[nR++] = A[i] */
    addl $1, %ecx
    movl %ecx, -52(%rbp)
 
    addl $1, -56(%rbp)          /* i++ */
    jmp FOR1
 
IF:
    movl -48(%rbp), %ecx        /* nL */
    movl %r11d, (%r13,%rcx,4)   /* L[nL++] = A[i] */
    addl $1, %ecx
    movl %ecx, -48(%rbp)
 
    addl $1, -56(%rbp)          /* i++ */
    jmp FOR1
 
POSFOR1:
    movq $SA, %rdi
    movq %rbx, %rsi
    movl %r12d, %edx
    call imprime        /* imprime("A", A, n) */
 
    movq $Sprintf, %rdi
    movl %r15d, %esi
    movb $0, %al
    call printf         /* printf("\nPivo = %d\n", p) */
 
    movq $SL, %rdi
    movq %r13, %rsi
    movl -48(%rbp), %edx
    call imprime        /* imprime("L", L, nL) */
 
    movq $SprintfVazia, %rdi
    movb $0, %al
    call printf         /* printf("   ") */
 
    movq $SR, %rdi
    movq %r14, %rsi
    movl -52(%rbp), %edx
    call imprime        /* imprime("R", R, nR) */
 
    movq $Sprintfn, %rdi
    movb $0, %al
    call printf         /* printf("\n\n") */
 
    movq %r13, %rdi
    movl -48(%rbp), %esi
    call quicksort      /* quicksort(L, nL) */
 
    movq %r14, %rdi
    movl -52(%rbp), %esi
    call quicksort      /* quicksort(R, nR) */
 
    movl $0, -56(%rbp)          /* i = 0 */
 
FOR2:
    movl -56(%rbp), %ecx        /* i */
    cmpl -48(%rbp), %ecx        /* i < nL */
    jge POSFOR2
 
    movl (%r13,%rcx,4), %r11d
    movl %r11d, (%rbx,%rcx,4)   /* A[i] = L[i] */
 
    addl $1, -56(%rbp)          /* i++ */
    jmp FOR2
 
POSFOR2:
    movl -48(%rbp), %ecx
    movl %r15d, (%rbx,%rcx,4)   /* A[nL] = p */
 
    movl $0, -56(%rbp)          /* i = 0 */
 
FOR3:
    movl -56(%rbp), %ecx        /* i */
    cmpl -52(%rbp), %ecx        /* i < nR */
    jge FIM
 
    movl (%r14,%rcx,4), %r11d   /* R[i] */
    movl -48(%rbp), %edx
    addl $1, %edx
    addl %ecx, %edx             /* nL + 1 + i */
    movl %r11d, (%rbx,%rdx,4)   /* A[nL + 1 + i] = R[i] */
 
    addl $1, -56(%rbp)          /* i++ */
    jmp FOR3
 
FIM:
    movq -8(%rbp), %rbx
    movq -16(%rbp), %r12
    movq -24(%rbp), %r13
    movq -32(%rbp), %r14
    movq -40(%rbp), %r15
 
    leave
    ret