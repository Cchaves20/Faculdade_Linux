#include <stdio.h>

void quicksort(int *A, int n);

void imprime(const char *nome, int *A, int n){
    printf("%s = [", nome);
    for (int k=0;k<n;k++) printf("%d%s",A[k],k<n-1?", ":"");
    printf("]");
}

int main(void){
    int A[] = {7,3,9,2,8,1,5,4,6};
    quicksort(A,9);
    imprime("Vetor ordenado", A, 9); printf("\n");
    return 0;
}