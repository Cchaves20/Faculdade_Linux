#include <stdio.h>
#include <signal.h>

void handler(int sig) {
    printf("Tentativa de capturar SIGKILL\n");
}

int main() {
    signal(SIGKILL, handler); // NÃO FUNCIONA

    while (1) {
        printf("Rodando...\n");
    }

    return 0;
}