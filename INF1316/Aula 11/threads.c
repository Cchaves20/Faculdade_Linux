#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define TAM 10000
#define NUM_THREADS 10

int vetor[TAM];

pthread_mutex_t mutex_inicio = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_inicio = PTHREAD_COND_INITIALIZER;
int pode_comecar = 0;

pthread_mutex_t mutex_fim = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_fim = PTHREAD_COND_INITIALIZER;
int terminaram = 0;

double tempo_ms(struct timeval inicio, struct timeval fim) {
    return (fim.tv_sec - inicio.tv_sec) * 1000.0 +
           (fim.tv_usec - inicio.tv_usec) / 1000.0;
}

void inicializa_vetor() {
    for (int i = 0; i < TAM; i++) {
        vetor[i] = 5;
    }
}

int verifica_tudo_igual(int v[]) {
    for (int i = 1; i < TAM; i++) {
        if (v[i] != v[0]) {
            return 0;
        }
    }
    return 1;
}

long long valor_esperado(int n) {
    long long valor = 5;

    for (int i = 0; i < n; i++) {
        valor = valor * 2 + 2;
    }

    return valor;
}

void imprime_resultado_igualdade(int valor) {
    if (valor == 1) {
        printf("Iguais 1\n");
    } else {
        printf("Iguais 0\n");
    }
}

void *trabalhadora(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex_inicio);
    while (!pode_comecar) {
        pthread_cond_wait(&cond_inicio, &mutex_inicio);
    }
    pthread_mutex_unlock(&mutex_inicio);

    for (int i = 0; i < TAM; i++) {
        vetor[i] = vetor[i] * 2 + 2;
    }

    pthread_mutex_lock(&mutex_fim);
    terminaram++;
    pthread_cond_signal(&cond_fim);
    pthread_mutex_unlock(&mutex_fim);

    printf("Thread %d\n", id + 1);

    return NULL;
}

int main(int argc, char *argv[]) {
    int n = NUM_THREADS;

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    if (n <= 0) {
        printf("Uso: %s [numero_de_threads]\n", argv[0]);
        return 1;
    }

    inicializa_vetor();

    pthread_t threads[n];
    int ids[n];
    struct timeval inicio, fim;

    for (int i = 0; i < n; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, trabalhadora, &ids[i]);
    }

    pthread_mutex_lock(&mutex_inicio);
    gettimeofday(&inicio, NULL);
    pode_comecar = 1;
    pthread_cond_broadcast(&cond_inicio);
    pthread_mutex_unlock(&mutex_inicio);

    pthread_mutex_lock(&mutex_fim);
    while (terminaram < n) {
        pthread_cond_wait(&cond_fim, &mutex_fim);
    }
    gettimeofday(&fim, NULL);
    pthread_mutex_unlock(&mutex_fim);

    printf("THREADS\n");
    printf("Valor %d\n", vetor[0]);
    imprime_resultado_igualdade(verifica_tudo_igual(vetor));
    printf("Esperado %lld\n", valor_esperado(n));
    printf("Tempo %.3f ms\n", tempo_ms(inicio, fim));

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}