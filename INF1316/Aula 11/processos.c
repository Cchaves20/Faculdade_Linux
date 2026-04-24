#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define TAM 10000
#define NUM_PROC 10

int vetor[TAM];

typedef struct {
    int filho;
    int valor;
    int tudo_igual;
} Resultado;

double tempo_ms(struct timeval inicio, struct timeval fim) {
    return (fim.tv_sec - inicio.tv_sec) * 1000.0 +
           (fim.tv_usec - inicio.tv_usec) / 1000.0;
}

int verifica_tudo_igual(int v[]) {
    for (int i = 1; i < TAM; i++) {
        if (v[i] != v[0]) {
            return 0;
        }
    }
    return 1;
}

void inicializa_vetor() {
    for (int i = 0; i < TAM; i++) {
        vetor[i] = 5;
    }
}

void imprime_resultado_igualdade(int valor) {
    if (valor == 1) {
        printf("Iguais 1\n");
    } else {
        printf("Iguais 0\n");
    }
}

int main(int argc, char *argv[]) {
    int n = NUM_PROC;

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    if (n <= 0) {
        printf("Uso: %s [numero_de_processos]\n", argv[0]);
        return 1;
    }

    inicializa_vetor();

    int pipe_inicio[2];
    int pipe_resultado[2];

    pipe(pipe_inicio);
    pipe(pipe_resultado);

    pid_t filhos[n];
    Resultado r;
    struct timeval inicio, fim;

    for (int i = 0; i < n; i++) {
        filhos[i] = fork();

        if (filhos[i] == 0) {
            close(pipe_inicio[1]);
            close(pipe_resultado[0]);

            char sinal;
            read(pipe_inicio[0], &sinal, 1);

            for (int j = 0; j < TAM; j++) {
                vetor[j] = vetor[j] * 2 + 2;
            }

            r.filho = i + 1;
            r.valor = vetor[0];
            r.tudo_igual = verifica_tudo_igual(vetor);

            write(pipe_resultado[1], &r, sizeof(Resultado));

            close(pipe_inicio[0]);
            close(pipe_resultado[1]);
            _exit(0);
        }
    }

    close(pipe_inicio[0]);
    close(pipe_resultado[1]);

    gettimeofday(&inicio, NULL);

    for (int i = 0; i < n; i++) {
        write(pipe_inicio[1], "A", 1);
    }

    printf("PROCESSOS\n");
    for (int i = 0; i < n; i++) {
        read(pipe_resultado[0], &r, sizeof(Resultado));
        printf("Filho %d\n", r.filho);
        printf("Valor %d\n", r.valor);
        imprime_resultado_igualdade(r.tudo_igual);
    }

    gettimeofday(&fim, NULL);

    printf("Pai\n");
    printf("Valor %d\n", vetor[0]);
    imprime_resultado_igualdade(verifica_tudo_igual(vetor));
    printf("Tempo %.3f ms\n", tempo_ms(inicio, fim));

    close(pipe_inicio[1]);
    close(pipe_resultado[0]);

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}