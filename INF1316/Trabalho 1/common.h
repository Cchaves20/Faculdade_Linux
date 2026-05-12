#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MIN_APPS 3
#define MAX_APPS 6

#define MAX_PC 10
#define TAM_FILA 64

#define TIME_SLICE 1
#define TEMPO_IO 3

#define SINAL_IRQ0 SIGUSR1
#define SINAL_IRQ1 SIGUSR2
#define SINAL_SYSCALL SIGRTMIN
#define SINAL_FIM (SIGRTMIN + 1)

#define NOVO 0
#define PRONTO 1
#define EXECUTANDO 2
#define BLOQUEADO_IO 3
#define FINALIZADO 4

typedef struct {
    pid_t pid;
    int id;
    int pc;
    int estado;
    int terminou;

    int syscall_pc;
    char syscall_op;
} PCB;

typedef struct {
    pid_t pid_kernel;
    pid_t pid_intercontroller;

    int n_apps;
    int vivos;
    int rodando;
    int tempo_simulacao;

    PCB apps[MAX_APPS];

    int fila_prontos[TAM_FILA];
    int prontos_ini;
    int prontos_fim;
    int prontos_qtd;

    int fila_bloqueados[TAM_FILA];
    int bloqueados_ini;
    int bloqueados_fim;
    int bloqueados_qtd;

    int syscall_app;
    int syscall_pc;
    char syscall_op;

    int encerrar;
} MemoriaCompartilhada;

static inline void erro(const char *msg) {
    perror(msg);
    exit(1);
}

static inline const char *nome_estado(int estado) {
    switch (estado) {
        case NOVO:
            return "NOVO";
        case PRONTO:
            return "PRONTO";
        case EXECUTANDO:
            return "EXECUTANDO";
        case BLOQUEADO_IO:
            return "BLOQUEADO_IO";
        case FINALIZADO:
            return "FINALIZADO";
        default:
            return "DESCONHECIDO";
    }
}

static inline void log_evento(int tempo, const char *processo, const char *explicacao, int pc) {
    if (pc >= 0) {
        printf("%05d | %-10s | %-70s | PC=%d\n", tempo, processo, explicacao, pc);
    } else {
        printf("%05d | %-10s | %-70s | PC=-\n", tempo, processo, explicacao);
    }
}

static inline void imprimir_cabecalho_log(void) {
    printf("Tempo | Processo   | Explicacao                                                             | PC\n");
    printf("------|------------|------------------------------------------------------------------------|------\n");
}

static inline void dormir_ms(long ms) {
    struct timespec tempo;

    tempo.tv_sec = ms / 1000;
    tempo.tv_nsec = (ms % 1000) * 1000000L;

    while (nanosleep(&tempo, &tempo) == -1 && errno == EINTR) {
    }
}

static inline MemoriaCompartilhada *abrir_memoria(const char *nome_memoria) {
    int fd = shm_open(nome_memoria, O_RDWR, 0600);

    if (fd == -1) {
        erro("shm_open");
    }

    void *addr = mmap(NULL,
                      sizeof(MemoriaCompartilhada),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fd,
                      0);

    if (addr == MAP_FAILED) {
        erro("mmap");
    }

    close(fd);

    return (MemoriaCompartilhada *)addr;
}

#endif
