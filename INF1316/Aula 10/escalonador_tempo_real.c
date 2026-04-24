#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

static pid_t p1 = -1, p2 = -1, p3 = -1;

void encerra_filho(int sinal) {
    (void)sinal;
    _exit(0);
}

void processo_periodico(const char *nome) {
    signal(SIGTERM, encerra_filho);
    raise(SIGSTOP);

    while (1) {
        printf("%s executando\n", nome);
        fflush(stdout);
        usleep(200000);
    }
}

void cria_filhos(void) {
    int status;

    p1 = fork();
    if (p1 < 0) {
        perror("fork p1");
        exit(1);
    }
    if (p1 == 0) {
        processo_periodico("P1");
    }
    waitpid(p1, &status, WUNTRACED);

    p2 = fork();
    if (p2 < 0) {
        perror("fork p2");
        kill(p1, SIGTERM);
        exit(1);
    }
    if (p2 == 0) {
        processo_periodico("P2");
    }
    waitpid(p2, &status, WUNTRACED);

    p3 = fork();
    if (p3 < 0) {
        perror("fork p3");
        kill(p1, SIGTERM);
        kill(p2, SIGTERM);
        exit(1);
    }
    if (p3 == 0) {
        processo_periodico("P3");
    }
    waitpid(p3, &status, WUNTRACED);
}

void para_todos(void) {
    if (p1 > 0) kill(p1, SIGSTOP);
    if (p2 > 0) kill(p2, SIGSTOP);
    if (p3 > 0) kill(p3, SIGSTOP);
}

void finaliza_todos(int sinal) {
    int status;
    (void)sinal;

    if (p1 > 0) kill(p1, SIGTERM);
    if (p2 > 0) kill(p2, SIGTERM);
    if (p3 > 0) kill(p3, SIGTERM);

    if (p1 > 0) waitpid(p1, &status, 0);
    if (p2 > 0) waitpid(p2, &status, 0);
    if (p3 > 0) waitpid(p3, &status, 0);

    exit(0);
}

int processo_desejado_no_segundo(int s) {
    if (s >= 0 && s < 5) return 3;
    if (s >= 5 && s < 25) return 1;
    if (s >= 25 && s < 45) return 3;
    return 2;
}

void espera_proximo_minuto_cheio(void) {
    struct timeval tv;

    while (1) {
        gettimeofday(&tv, NULL);
        if ((tv.tv_sec % 60) == 0)
            return;
        usleep(100000);
    }
}

int main(void) {
    int atual = 0;
    int desejado;
    struct timeval tv;

    setvbuf(stdout, NULL, _IOLBF, 0);

    signal(SIGINT, finaliza_todos);
    signal(SIGTERM, finaliza_todos);

    cria_filhos();
    espera_proximo_minuto_cheio();

    while (1) {
        gettimeofday(&tv, NULL);
        desejado = processo_desejado_no_segundo((int)(tv.tv_sec % 60));

        if (desejado != atual) {
            para_todos();

            if (desejado == 1) kill(p1, SIGCONT);
            else if (desejado == 2) kill(p2, SIGCONT);
            else kill(p3, SIGCONT);

            atual = desejado;
        }

        usleep(100000);
    }

    return 0;
}