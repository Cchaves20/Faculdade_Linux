#include "common.h"

static const int pcs_syscall[MAX_APPS][4] = {
    {3, 7, -1, -1},
    {4, -1, -1, -1},
    {2, 6, -1, -1},
    {5, -1, -1, -1},
    {3, 8, -1, -1},
    {2, 9, -1, -1}
};

static const char ops_syscall[MAX_APPS][4] = {
    {'R', 'W', 0, 0},
    {'W', 0, 0, 0},
    {'R', 'W', 0, 0},
    {'R', 0, 0, 0},
    {'W', 'R', 0, 0},
    {'R', 'W', 0, 0}
};

static int deve_fazer_syscall(int id, int pc, char *op) {
    int i;

    for (i = 0; i < 4; i++) {
        if (pcs_syscall[id][i] == pc) {
            *op = ops_syscall[id][i];
            return 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    int id;
    int pc;

    char op;
    char nome_app[16];
    char explicacao[128];

    const char *nome_memoria;
    pid_t pid_kernel;

    MemoriaCompartilhada *mem;

    if (argc != 4) {
        printf("Uso interno: app <id> <nome_memoria> <pid_kernel>\n");
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    id = atoi(argv[1]);
    nome_memoria = argv[2];
    pid_kernel = (pid_t)atoi(argv[3]);

    mem = abrir_memoria(nome_memoria);

    snprintf(nome_app, sizeof(nome_app), "A%d", id + 1);
    snprintf(explicacao, sizeof(explicacao), "criado e aguardando primeira execucao. PID=%d", getpid());
    log_evento(mem->tempo_simulacao, nome_app, explicacao, -1);

    raise(SIGSTOP);

    for (pc = 1; pc <= MAX_PC; pc++) {
        mem->apps[id].pc = pc;

        snprintf(explicacao, sizeof(explicacao), "executando instrucao da aplicacao. PID=%d", getpid());
        log_evento(mem->tempo_simulacao, nome_app, explicacao, pc);

        sleep(1);

        if (deve_fazer_syscall(id, pc, &op)) {
            snprintf(explicacao, sizeof(explicacao), "chamou syscall(D1,%c)", op);
            log_evento(mem->tempo_simulacao, nome_app, explicacao, pc);

            mem->syscall_app = id;
            mem->syscall_pc = pc;
            mem->syscall_op = op;
            mem->apps[id].estado = BLOQUEADO_IO;

            kill(pid_kernel, SINAL_SYSCALL);

            while (mem->apps[id].estado != EXECUTANDO &&
                   mem->apps[id].estado != FINALIZADO) {
                dormir_ms(50);
            }
        }
    }

    mem->apps[id].pc = MAX_PC;

    snprintf(explicacao, sizeof(explicacao), "terminou todas as %d iteracoes", MAX_PC);
    log_evento(mem->tempo_simulacao, nome_app, explicacao, MAX_PC);

    kill(pid_kernel, SINAL_FIM);

    munmap(mem, sizeof(MemoriaCompartilhada));

    return 0;
}
