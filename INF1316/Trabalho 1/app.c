#include "common.h"

#define MAX_SYSCALLS 4

static const int pcs_syscall[MAX_APPS][MAX_SYSCALLS] = {
    {3, 7, -1, -1},
    {4, -1, -1, -1},
    {2, 6, -1, -1},
    {5, -1, -1, -1},
    {3, 8, -1, -1},
    {2, 9, -1, -1}
};

static const char ops_syscall[MAX_APPS][MAX_SYSCALLS] = {
    {'R', 'W', 0, 0},
    {'W', 0, 0, 0},
    {'R', 'W', 0, 0},
    {'R', 0, 0, 0},
    {'W', 'R', 0, 0},
    {'R', 'W', 0, 0}
};

static int syscall_do_pc(int id, int pc, char *op) {
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        if (pcs_syscall[id][i] == pc) {
            *op = ops_syscall[id][i];
            return 1;
        }
    }

    return 0;
}

static void avisar_syscall(MemoriaCompartilhada *mem,
                           pid_t pid_kernel,
                           int id,
                           int pc,
                           char op) {
    mem->syscall_app = id;
    mem->syscall_pc = pc;
    mem->syscall_op = op;

    kill(pid_kernel, SINAL_SYSCALL);
    raise(SIGSTOP);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Uso interno: app <id> <nome_memoria> <pid_kernel>\n");
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    int id = atoi(argv[1]);
    const char *nome_memoria = argv[2];
    pid_t pid_kernel = (pid_t)atoi(argv[3]);

    MemoriaCompartilhada *mem = abrir_memoria(nome_memoria);

    char nome_processo[16];
    snprintf(nome_processo, sizeof(nome_processo), "A%d", id + 1);

    log_evento(mem->tempo_simulacao,
               nome_processo,
               "processo criado e parado inicialmente",
               0);

    raise(SIGSTOP);

    for (int pc = 1; pc <= MAX_PC; pc++) {
        char op;

        mem->apps[id].pc = pc;
        log_evento(mem->tempo_simulacao, nome_processo, "executando na CPU", pc);

        sleep(1);

        if (syscall_do_pc(id, pc, &op)) {
            char texto[80];

            snprintf(texto, sizeof(texto), "solicitou syscall(D1,%c)", op);
            log_evento(mem->tempo_simulacao, nome_processo, texto, pc);

            avisar_syscall(mem, pid_kernel, id, pc, op);
        }
    }

    mem->apps[id].pc = MAX_PC;
    log_evento(mem->tempo_simulacao,
               nome_processo,
               "terminou todas as iteracoes",
               MAX_PC);

    kill(pid_kernel, SINAL_FIM);
    munmap(mem, sizeof(MemoriaCompartilhada));

    return 0;
}
