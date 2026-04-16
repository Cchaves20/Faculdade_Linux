#include <stdio.h>

int boba(float v[], float lim, int n);

int main(void) {
    /* Exemplo de vetor e limite */
    float v[] = { 1.2f, 4.5f, 3.0f, 6.7f, 2.9f };
    float limite = 3.5f;
    int n = sizeof(v) / sizeof(v[0]);

    /* Chama boba e armazena o número de elementos > limite */
    int qtd = boba(v, limite, n);

    /* Exibe o resultado */
    printf("Há %d elemento(s) maior(es) que %.2f no vetor.\n", qtd, limite);

    return 0;
}