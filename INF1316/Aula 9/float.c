#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void trata_fpe(int sig) {
    printf("\nRecebi SIGFPE (erro aritmetico).\n");
    exit(1);
}

int main() {
    double a, b;

    signal(SIGFPE, trata_fpe);

    printf("Digite dois numeros reais: ");
    if (scanf("%lf %lf", &a, &b) != 2) {
        printf("Entrada invalida.\n");
        return 1;
    }

    printf("Soma: %.2f\n", a + b);
    printf("Subtracao: %.2f\n", a - b);
    printf("Multiplicacao: %.2f\n", a * b);
    printf("Divisao: %.2f\n", a / b);

    return 0;
}