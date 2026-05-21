#include "common.h"

static int syscall_do_pc(int n_apps, int id, int pc, char *op) {
    if (n_apps == 3) {
        return 0; // cenário 1: A1, A2 e A3 sem I/O
    }

    if (n_apps == 6) {
        if (id == 2 && pc == 2) { // A3
            *op = 'R';
            return 1;
        }

        if (id == 2 && pc == 6) { // A3
            *op = 'W';
            return 1;
        }

        if (id == 5 && pc == 2) { // A6
            *op = 'R';
            return 1;
        }

        if (id == 5 && pc == 9) { // A6
            *op = 'W';
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

        if (syscall_do_pc(mem->n_apps, id, pc, &op)) {
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