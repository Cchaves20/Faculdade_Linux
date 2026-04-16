#include <stdio.h>

/* Protótipos das funções externas */
int    calculaIndice(int valor);
double trata(int u);
double foo(int v[], int n);

int calculaIndice(int valor) {
    /* Exemplo simples: retorna o próprio valor */
    return valor;
}

/* --- Stub de trata, caso não esteja em outro módulo --- */
double trata(int u) {
    return (double)u;
}

int main(void) {
    /* Exemplo de vetor e tamanho */
    int  vetor[] = { 2, 4, 6, 8, 10 };
    int  n      = sizeof(vetor) / sizeof(vetor[0]);
    double resultado;

    /* Chama foo e imprime o resultado */
    resultado = foo(vetor, n);
    printf("foo(vetor, %d) = %.2f\n", n, resultado);

    return 0;
}
