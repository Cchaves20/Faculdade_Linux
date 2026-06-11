#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM 10000
#define NUM_TAREFAS 100

int vetor[TAM];

pthread_mutex_t mutex_inicio = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_inicio = PTHREAD_COND_INITIALIZER;
int pode_comecar = 0;

void inicializa_vetor() {
    for (int i = 0; i < TAM; i++) {
        vetor[i] = 10;
    }
}

/* Tarefa i (1..n): impar subtrai i, par soma i em todas as posicoes */
void *tarefa(void *arg) {
    int id = *(int *)arg;
    int valor = (id % 2 == 0) ? id : -id;

    pthread_mutex_lock(&mutex_inicio);
    while (!pode_comecar) {
        pthread_cond_wait(&cond_inicio, &mutex_inicio);
    }
    pthread_mutex_unlock(&mutex_inicio);

    for (int i = 0; i < TAM; i++) {
        vetor[i] = vetor[i] + valor;
    }

    return NULL;
}

long long valor_esperado(int n) {
    long long valor = 10;

    for (int i = 1; i <= n; i++) {
        valor += (i % 2 == 0) ? i : -i;
    }

    return valor;
}

int main(int argc, char *argv[]) {
    int n = NUM_TAREFAS;

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    if (n <= 0) {
        printf("Uso: %s [numero_de_tarefas]\n", argv[0]);
        return 1;
    }

    inicializa_vetor();

    pthread_t threads[n];
    int ids[n];

    for (int i = 0; i < n; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, tarefa, &ids[i]);
    }

    pthread_mutex_lock(&mutex_inicio);
    pode_comecar = 1;
    pthread_cond_broadcast(&cond_inicio);
    pthread_mutex_unlock(&mutex_inicio);

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    long long esperado = valor_esperado(n);
    int erros = 0;

    printf("SEM SEMAFORO (%d tarefas)\n", n);
    printf("Valor esperado em cada posicao: %lld\n", esperado);

    for (int i = 0; i < TAM; i++) {
        if (vetor[i] != esperado) {
            if (erros < 20) {
                printf("Erro na posicao %d: valor %d (esperado %lld)\n",
                       i, vetor[i], esperado);
            }
            erros++;
        }
    }

    if (erros > 20) {
        printf("... (apenas as 20 primeiras posicoes com erro foram listadas)\n");
    }

    printf("Total de posicoes com erro: %d de %d\n", erros, TAM);

    if (erros == 0) {
        printf("Nao houve erro de concorrencia. Tente aumentar o numero de tarefas (ex: %s 1000)\n", argv[0]);
    } else {
        printf("Houve erro de concorrencia!\n");
    }

    return 0;
}
