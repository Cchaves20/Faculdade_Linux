#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void trata_sigint(int sig) {
    printf("\nRecebi SIGINT (Ctrl-C)\n");
}

void trata_sigquit(int sig) {
    printf("\nRecebi SIGQUIT (Ctrl-\\)\n");
}

int main() {
    signal(SIGINT, trata_sigint);
    signal(SIGQUIT, trata_sigquit);

    while (1) {
        printf("Executando... (PID=%d)\n", getpid());
        sleep(2);
    }

    return 0;
}