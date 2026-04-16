#include <stdio.h>

/* Protótipo de trata */
double trata(int u);

int main(void) {
    int  valor = 42;
    double resultado;

    /* Chama trata e armazena o retorno */
    resultado = trata(valor);

    /* Imprime o valor convertido */
    printf("trata(%d) = %.2f\n", valor, resultado);

    return 0;
}
