#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main (void) 
{
    int tamanho = 10000;
    int shmid;
    int *vetor;

    shmid = shmget(IPC_PRIVATE, tamanho * sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    vetor = (int*)shmat(shmid, NULL, 0);

    for (int i = 0; i < tamanho; i++) {
        vetor[i] = 1;
    }

    int filho1 = fork();
    if (filho1 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho2 = fork();
    if (filho2 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho3 = fork();
    if (filho3 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho4 = fork();
    if (filho4 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho5 = fork();
    if (filho5 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho6 = fork();
    if (filho6 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho7 = fork();
    if (filho7 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho8 = fork();
    if (filho8 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho9 = fork();
    if (filho9 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    int filho10 = fork();
    if (filho10 == 0) { 
        for (int j = 0; j < tamanho; j++) vetor[j] = vetor[j] * 2 + 2;
        exit(0);
    }

    for (int i = 0; i < 10; i++) {
        wait(NULL);
    }

    int inconsistencias = 0;
    int valor_referencia = vetor[0];

    for (int i = 0; i < tamanho; i++) {
        if (vetor[i] != valor_referencia) {
            inconsistencias++;
        }
    }

    if (inconsistencias == 0) {
        printf("Todas as posicoes tem o valor: %d\n", valor_referencia);
    } else {
        printf("Erro: Encontradas %d inconsistencias no vetor!\n", inconsistencias);
    }

    shmdt(vetor);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}