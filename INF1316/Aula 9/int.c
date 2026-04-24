#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void trata_fpe(int sig) {
    printf("\nErro: divisao por zero!\n");
    exit(1);
}

int main() {
    int a, b;

    signal(SIGFPE, trata_fpe);

    printf("Digite dois inteiros: ");
    scanf("%d %d", &a, &b);

    printf("Soma: %d\n", a + b);
    printf("Subtracao: %d\n", a - b);
    printf("Multiplicacao: %d\n", a * b);
    printf("Divisao: %d\n", a / b);

    return 0;
}