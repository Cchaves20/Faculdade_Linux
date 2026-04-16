#include <stdio.h>

/* Protótipo da função foo */
void foo(double *vd, int n);

int main(void) {
    /* Exemplo de vetor */
    double vetor[] = { 3.14, -2.0, 42.42, 1.618, -0.5772 };
    int n = sizeof(vetor) / sizeof(vetor[0]);

    /* Imprime valores antes de chamar foo */
    printf("Antes de foo:\n");
    for (int i = 0; i < n; i++) {
        printf("  vetor[%d] = %.4f\n", i, vetor[i]);
    }

    /* Chama foo para zerar todos os elementos */
    foo(vetor, n);

    /* Imprime valores depois de chamar foo */
    printf("\nDepois de foo:\n");
    for (int i = 0; i < n; i++) {
        printf("  vetor[%d] = %.4f\n", i, vetor[i]);
    }

    return 0;
}
