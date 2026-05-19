#include "common.h"

#include <sys/wait.h>

static int fila_tem(int fila[], int ini, int qtd, int valor) {
    for (int i = 0; i < qtd; i++) {
        if (fila[(ini + i) % TAM_FILA] == valor) {
            return 1;
        }
    }

    return 0;
}

static void enfileirar(int fila[], int *fim, int *qtd, int valor) {
    if (*qtd >= TAM_FILA) {
        log_evento(0, "KERNEL", "erro: fila cheia", -1);
        exit(1);
    }

    fila[*fim] = valor;
    *fim = (*fim + 1) % TAM_FILA;
    (*qtd)++;
}

static int desenfileirar(int fila[], int *ini, int *qtd) {
    if (*qtd == 0) {
        return -1;
    }

    int valor = fila[*ini];

    *ini = (*ini + 1) % TAM_FILA;
    (*qtd)--;

    return valor;
}

static void colocar_pronto(MemoriaCompartilhada *mem, int id) {
    if (id < 0 || id >= mem->n_apps) {
        return;
    }

    if (mem->apps[id].estado == FINALIZADO ||
        mem->apps[id].estado == BLOQUEADO_IO ||
        fila_tem(mem->fila_prontos, mem->prontos_ini, mem->prontos_qtd, id)) {
        return;
    }

    enfileirar(mem->fila_prontos, &mem->prontos_fim, &mem->prontos_qtd, id);
}

static int tirar_pronto(MemoriaCompartilhada *mem) {
    while (mem->prontos_qtd > 0) {
        int id = desenfileirar(mem->fila_prontos,
                               &mem->prontos_ini,
                               &mem->prontos_qtd);

        if (id >= 0 && mem->apps[id].estado == PRONTO) {
            return id;
        }
    }

    return -1;
}

static void colocar_bloqueado(MemoriaCompartilhada *mem, int id) {
    enfileirar(mem->fila_bloqueados,
               &mem->bloqueados_fim,
               &mem->bloqueados_qtd,
               id);
}

static int tirar_bloqueado(MemoriaCompartilhada *mem) {
    return desenfileirar(mem->fila_bloqueados,
                         &mem->bloqueados_ini,
                         &mem->bloqueados_qtd);
}

static void nome_app(int id, char *destino, size_t tamanho) {
    snprintf(destino, tamanho, "A%d", id + 1);
}

static int buscar_app_por_pid(MemoriaCompartilhada *mem, pid_t pid) {
    for (int i = 0; i < mem->n_apps; i++) {
        if (mem->apps[i].pid == pid) {
            return i;
        }
    }

    return -1;
}

static void imprimir_tabela(MemoriaCompartilhada *mem) {
    char texto[256] = "";
    int usado = 0;

    for (int i = 0; i < mem->n_apps && usado < (int)sizeof(texto); i++) {
        usado += snprintf(texto + usado,
                          sizeof(texto) - (size_t)usado,
                          "A%d{%s,PC=%d} ",
                          i + 1,
                          nome_estado(mem->apps[i].estado),
                          mem->apps[i].pc);
    }

    log_evento(mem->tempo_simulacao, "KERNEL", texto, -1);
}

static void escalonar_proximo(MemoriaCompartilhada *mem) {
    int id = tirar_pronto(mem);

    if (id == -1) {
        mem->rodando = -1;
        log_evento(mem->tempo_simulacao,
                   "KERNEL",
                   "nenhum processo pronto: CPU ociosa",
                   -1);
        return;
    }

    char processo[16];

    mem->rodando = id;
    mem->apps[id].estado = EXECUTANDO;

    nome_app(id, processo, sizeof(processo));
    log_evento(mem->tempo_simulacao,
               processo,
               "escalonado pelo KernelSim com SIGCONT",
               mem->apps[id].pc);

    kill(mem->apps[id].pid, SIGCONT);
}

static void parar_atual(MemoriaCompartilhada *mem) {
    int id = mem->rodando;
    char processo[16];

    if (id == -1 || mem->apps[id].estado != EXECUTANDO) {
        return;
    }

    nome_app(id, processo, sizeof(processo));
    log_evento(mem->tempo_simulacao,
               processo,
               "interrompido pelo KernelSim com SIGSTOP",
               mem->apps[id].pc);

    kill(mem->apps[id].pid, SIGSTOP);

    mem->apps[id].estado = PRONTO;
    colocar_pronto(mem, id);
    mem->rodando = -1;
}

static void tratar_irq0(MemoriaCompartilhada *mem) {
    log_evento(mem->tempo_simulacao,
               "KERNEL",
               "recebeu IRQ0: fim do time-slice",
               -1);

    parar_atual(mem);
    escalonar_proximo(mem);
    imprimir_tabela(mem);
}

static void tratar_irq1(MemoriaCompartilhada *mem) {
    int id = tirar_bloqueado(mem);

    log_evento(mem->tempo_simulacao,
               "KERNEL",
               "recebeu IRQ1: fim de E/S em D1",
               -1);

    if (id == -1) {
        log_evento(mem->tempo_simulacao,
                   "KERNEL",
                   "IRQ1 ignorado: nenhum processo bloqueado",
                   -1);
        return;
    }

    char processo[16];

    nome_app(id, processo, sizeof(processo));
    log_evento(mem->tempo_simulacao,
               processo,
               "E/S terminou, voltou para fila de prontos",
               mem->apps[id].syscall_pc);

    mem->apps[id].estado = PRONTO;
    colocar_pronto(mem, id);

    if (mem->rodando == -1) {
        escalonar_proximo(mem);
    }

    imprimir_tabela(mem);
}

static void tratar_syscall(MemoriaCompartilhada *mem, pid_t pid_emissor) {
    int id = mem->syscall_app;

    if (id < 0 || id >= mem->n_apps) {
        id = buscar_app_por_pid(mem, pid_emissor);
    }

    if (id == -1) {
        log_evento(mem->tempo_simulacao,
                   "KERNEL",
                   "syscall ignorada: processo desconhecido",
                   -1);
        return;
    }

    char processo[16];
    char texto[80];

    nome_app(id, processo, sizeof(processo));
    snprintf(texto, sizeof(texto), "KernelSim recebeu syscall(D1,%c)", mem->syscall_op);

    log_evento(mem->tempo_simulacao, processo, texto, mem->syscall_pc);

    mem->apps[id].estado = BLOQUEADO_IO;
    mem->apps[id].syscall_op = mem->syscall_op;
    mem->apps[id].syscall_pc = mem->syscall_pc;

    colocar_bloqueado(mem, id);

    log_evento(mem->tempo_simulacao,
               processo,
               "bloqueado aguardando E/S em D1",
               mem->apps[id].pc);

    kill(mem->apps[id].pid, SIGSTOP);

    if (mem->rodando == id) {
        mem->rodando = -1;
    }

    mem->syscall_app = -1;
    mem->syscall_pc = 0;
    mem->syscall_op = 0;

    if (mem->rodando == -1) {
        escalonar_proximo(mem);
    }

    imprimir_tabela(mem);
}

static void tratar_fim(MemoriaCompartilhada *mem, pid_t pid_emissor) {
    int id = buscar_app_por_pid(mem, pid_emissor);

    if (id == -1) {
        return;
    }

    char processo[16];

    nome_app(id, processo, sizeof(processo));
    log_evento(mem->tempo_simulacao,
               processo,
               "processo finalizado",
               mem->apps[id].pc);

    mem->apps[id].estado = FINALIZADO;
    mem->apps[id].terminou = 1;
    mem->vivos--;

    if (mem->rodando == id) {
        mem->rodando = -1;
    }

    if (mem->vivos > 0 && mem->rodando == -1) {
        escalonar_proximo(mem);
    }

    imprimir_tabela(mem);
}

static MemoriaCompartilhada *criar_memoria(const char *nome_memoria) {
    int fd = shm_open(nome_memoria, O_CREAT | O_RDWR, 0600);

    if (fd == -1) {
        erro("shm_open");
    }

    if (ftruncate(fd, sizeof(MemoriaCompartilhada)) == -1) {
        erro("ftruncate");
    }

    void *addr = mmap(NULL, sizeof(MemoriaCompartilhada),
                      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        erro("mmap");
    }

    close(fd);
    memset(addr, 0, sizeof(MemoriaCompartilhada));

    return addr;
}

static void criar_app(MemoriaCompartilhada *mem,
                      const char *nome_memoria,
                      int id) {
    pid_t pid = fork();

    if (pid == -1) {
        erro("fork app");
    }

    if (pid == 0) {
        char id_txt[16];
        char pid_kernel_txt[32];

        snprintf(id_txt, sizeof(id_txt), "%d", id);
        snprintf(pid_kernel_txt, sizeof(pid_kernel_txt), "%d", mem->pid_kernel);

        execl("./app", "app", id_txt, nome_memoria, pid_kernel_txt, NULL);
        erro("execl app");
    }

    mem->apps[id] = (PCB) {
        .pid = pid,
        .id = id,
        .pc = 0,
        .estado = PRONTO,
        .terminou = 0,
        .syscall_pc = 0,
        .syscall_op = 0
    };

    colocar_pronto(mem, id);
}

static void criar_intercontroller(MemoriaCompartilhada *mem,
                                  const char *nome_memoria) {
    pid_t pid = fork();

    if (pid == -1) {
        erro("fork intercontroller");
    }

    if (pid == 0) {
        char pid_kernel_txt[32];

        snprintf(pid_kernel_txt, sizeof(pid_kernel_txt), "%d", mem->pid_kernel);

        execl("./intercontroller",
              "intercontroller",
              nome_memoria,
              pid_kernel_txt,
              NULL);

        erro("execl intercontroller");
    }

    mem->pid_intercontroller = pid;
}

static void preparar_sinais(sigset_t *sinais) {
    sigemptyset(sinais);
    sigaddset(sinais, SINAL_IRQ0);
    sigaddset(sinais, SINAL_IRQ1);
    sigaddset(sinais, SINAL_SYSCALL);
    sigaddset(sinais, SINAL_FIM);

    if (sigprocmask(SIG_BLOCK, sinais, NULL) == -1) {
        erro("sigprocmask");
    }
}

static void finalizar(MemoriaCompartilhada *mem, const char *nome_memoria) {
    log_evento(mem->tempo_simulacao,
               "KERNEL",
               "todos os processos de aplicacao terminaram",
               -1);

    log_evento(mem->tempo_simulacao,
               "KERNEL",
               "encerrando InterControllerSim",
               -1);

    mem->encerrar = 1;
    kill(mem->pid_intercontroller, SIGTERM);

    for (int i = 0; i < mem->n_apps; i++) {
        waitpid(mem->apps[i].pid, NULL, 0);
    }

    waitpid(mem->pid_intercontroller, NULL, 0);

    munmap(mem, sizeof(MemoriaCompartilhada));
    shm_unlink(nome_memoria);

    log_evento(0, "KERNEL", "fim da simulacao", -1);
}

int main(int argc, char **argv) {
    int n_apps = (argc >= 2) ? atoi(argv[1]) : 3;

    if (n_apps < MIN_APPS || n_apps > MAX_APPS) {
        printf("Uso: %s <quantidade_de_apps_3_a_6>\n", argv[0]);
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    char nome_memoria[64];
    snprintf(nome_memoria, sizeof(nome_memoria), "/trab1_so_%d", getpid());

    MemoriaCompartilhada *mem = criar_memoria(nome_memoria);

    mem->pid_kernel = getpid();
    mem->n_apps = n_apps;
    mem->vivos = n_apps;
    mem->rodando = -1;
    mem->syscall_app = -1;

    sigset_t sinais;
    preparar_sinais(&sinais);

    imprimir_cabecalho_linha_do_tempo();

    log_evento(0, "KERNEL", "KernelSim iniciado", -1);
    log_evento(0, "KERNEL", "memoria compartilhada criada", -1);

    for (int i = 0; i < n_apps; i++) {
        criar_app(mem, nome_memoria, i);
        sleep(1);
    }

    criar_intercontroller(mem, nome_memoria);
    dormir_ms(500);

    log_evento(mem->tempo_simulacao,
               "KERNEL",
               "comecando escalonamento Round Robin",
               -1);

    escalonar_proximo(mem);

    while (mem->vivos > 0) {
        siginfo_t info;
        int sinal = sigwaitinfo(&sinais, &info);

        if (sinal == -1) {
            if (errno == EINTR) {
                continue;
            }

            erro("sigwaitinfo");
        }

        if (sinal == SINAL_IRQ0) {
            tratar_irq0(mem);
        } else if (sinal == SINAL_IRQ1) {
            tratar_irq1(mem);
        } else if (sinal == SINAL_SYSCALL) {
            tratar_syscall(mem, info.si_pid);
        } else if (sinal == SINAL_FIM) {
            tratar_fim(mem, info.si_pid);
        }
    }

    finalizar(mem, nome_memoria);

    return 0;
}
