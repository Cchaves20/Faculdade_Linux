#include <stdio.h>

/* implementado em bubblesort.s */
void bubbleSort(int *A, int n);

/* imprime(int i, int *A, int n)  ->  %edi = i, %rsi = A, %edx = n */
void imprime(int i, int *A, int n) {
    printf("A^(%d) = [", i);
    for (int k = 0; k < n; k++)
        printf("%d%s", A[k], k < n - 1 ? ", " : "");
    printf("]\n");
}

int main(void) {
    int A[] = {7, 3, 9, 2, 8, 1, 5, 4, 6};
    int B[] = {12, 4, 17, 4, 9, 20, 1, 15, 8, 3, 11, 6, 19, 2, 10};

    int nA = (int) (sizeof(A) / sizeof(A[0]));   /* 9  */
    int nB = (int) (sizeof(B) / sizeof(B[0]));   /* 15 */

    printf("=== Teste 1: vetor com %d elementos ===\n", nA);
    bubbleSort(A, nA);
    printf("\n=== Teste 2: vetor com %d elementos ===\n", nB);
    bubbleSort(B, nB);

    return 0;
}