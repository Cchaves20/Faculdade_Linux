#include "common.h"

#include <sys/wait.h>

static int existe_na_fila(int fila[], int ini, int qtd, int valor) {
    int i;

    for (i = 0; i < qtd; i++) {
        int pos = (ini + i) % TAM_FILA;

        if (fila[pos] == valor) {
            return 1;
        }
    }

    return 0;
}

static void enfileirar(int fila[], int *ini, int *fim, int *qtd, int valor) {
    (void)ini;

    if (*qtd >= TAM_FILA) {
        log_evento(0, "KERNEL", "erro: fila cheia", -1);
        exit(1);
    }

    fila[*fim] = valor;
    *fim = (*fim + 1) % TAM_FILA;
    (*qtd)++;
}

static int desenfileirar(int fila[], int *ini, int *fim, int *qtd) {
    int valor;

    (void)fim;

    if (*qtd == 0) {
        return -1;
    }

    valor = fila[*ini];
    *ini = (*ini + 1) % TAM_FILA;
    (*qtd)--;

    return valor;
}

static void colocar_pronto(MemoriaCompartilhada *mem, int id) {
    if (id < 0 || id >= mem->n_apps) {
        return;
    }

    if (mem->apps[id].estado == FINALIZADO ||
        mem->apps[id].estado == BLOQUEADO_IO) {
        return;
    }

    if (existe_na_fila(mem->fila_prontos,
                       mem->prontos_ini,
                       mem->prontos_qtd,
                       id)) {
        return;
    }

    enfileirar(mem->fila_prontos,
               &mem->prontos_ini,
               &mem->prontos_fim,
               &mem->prontos_qtd,
               id);
}

static int tirar_pronto(MemoriaCompartilhada *mem) {
    while (mem->prontos_qtd > 0) {
        int id = desenfileirar(mem->fila_prontos,
                               &mem->prontos_ini,
                               &mem->prontos_fim,
                               &mem->prontos_qtd);

        if (id >= 0 && mem->apps[id].estado == PRONTO) {
            return id;
        }
    }

    return -1;
}

static void colocar_bloqueado(MemoriaCompartilhada *mem, int id) {
    enfileirar(mem->fila_bloqueados,
               &mem->bloqueados_ini,
               &mem->bloqueados_fim,
               &mem->bloqueados_qtd,
               id);
}

static int tirar_bloqueado(MemoriaCompartilhada *mem) {
    return desenfileirar(mem->fila_bloqueados,
                         &mem->bloqueados_ini,
                         &mem->bloqueados_fim,
                         &mem->bloqueados_qtd);
}

static void imprimir_tabela(MemoriaCompartilhada *mem) {
    int i;
    char explicacao[1024];
    int usados;

    usados = snprintf(explicacao, sizeof(explicacao), "Tabela de processos: ");

    for (i = 0; i < mem->n_apps && usados < (int)sizeof(explicacao); i++) {
        usados += snprintf(explicacao + usados,
                           sizeof(explicacao) - usados,
                           "A%d{pid=%d, PC=%d, %s} ",
                           i + 1,
                           mem->apps[i].pid,
                           mem->apps[i].pc,
                           nome_estado(mem->apps[i].estado));
    }

    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, -1);
}

static void escalonar_proximo(MemoriaCompartilhada *mem) {
    int proximo = tirar_pronto(mem);
    char explicacao[128];

    if (proximo == -1) {
        mem->rodando = -1;
        log_evento(mem->tempo_simulacao, "KERNEL", "nenhum processo pronto; CPU ociosa", -1);
        return;
    }

    mem->rodando = proximo;
    mem->apps[proximo].estado = EXECUTANDO;

    snprintf(explicacao,
             sizeof(explicacao),
             "escalonou A%d e enviou SIGCONT",
             proximo + 1);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[proximo].pc);

    kill(mem->apps[proximo].pid, SIGCONT);
}

static void tratar_irq0(MemoriaCompartilhada *mem) {
    int atual = mem->rodando;
    char explicacao[128];

    log_evento(mem->tempo_simulacao, "KERNEL", "recebeu IRQ0: fim do time-slice", -1);

    if (atual != -1 && mem->apps[atual].estado == EXECUTANDO) {
        snprintf(explicacao,
                 sizeof(explicacao),
                 "interrompeu A%d por fim de time-slice e enviou SIGSTOP",
                 atual + 1);
        log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[atual].pc);

        kill(mem->apps[atual].pid, SIGSTOP);

        mem->apps[atual].estado = PRONTO;
        colocar_pronto(mem, atual);
    }

    mem->rodando = -1;

    escalonar_proximo(mem);
    imprimir_tabela(mem);
}

static void tratar_irq1(MemoriaCompartilhada *mem) {
    int id;
    char explicacao[128];

    log_evento(mem->tempo_simulacao, "KERNEL", "recebeu IRQ1: fim de E/S em D1", -1);

    id = tirar_bloqueado(mem);

    if (id == -1) {
        log_evento(mem->tempo_simulacao, "KERNEL", "IRQ1 ignorado: nao ha processo bloqueado em D1", -1);
        return;
    }

    snprintf(explicacao,
             sizeof(explicacao),
             "E/S terminou para A%d; syscall(D1,%c)",
             id + 1,
             mem->apps[id].syscall_op);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[id].syscall_pc);

    mem->apps[id].estado = PRONTO;
    colocar_pronto(mem, id);

    snprintf(explicacao, sizeof(explicacao), "desbloqueou A%d e colocou na fila de prontos", id + 1);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[id].pc);

    if (mem->rodando == -1) {
        escalonar_proximo(mem);
    }

    imprimir_tabela(mem);
}

static void tratar_syscall(MemoriaCompartilhada *mem, pid_t pid_emissor) {
    int id = mem->syscall_app;
    int i;
    char explicacao[128];

    if (id < 0 || id >= mem->n_apps) {
        for (i = 0; i < mem->n_apps; i++) {
            if (mem->apps[i].pid == pid_emissor) {
                id = i;
                break;
            }
        }
    }

    if (id < 0 || id >= mem->n_apps) {
        log_evento(mem->tempo_simulacao, "KERNEL", "syscall ignorada: processo desconhecido", -1);
        return;
    }

    snprintf(explicacao,
             sizeof(explicacao),
             "recebeu syscall de A%d: syscall(D1,%c)",
             id + 1,
             mem->syscall_op);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->syscall_pc);

    mem->apps[id].estado = BLOQUEADO_IO;
    mem->apps[id].syscall_op = mem->syscall_op;
    mem->apps[id].syscall_pc = mem->syscall_pc;

    colocar_bloqueado(mem, id);

    snprintf(explicacao,
             sizeof(explicacao),
             "bloqueou A%d em D1 e enviou SIGSTOP",
             id + 1);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[id].pc);

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
    int i;
    int id = -1;
    char explicacao[128];

    for (i = 0; i < mem->n_apps; i++) {
        if (mem->apps[i].pid == pid_emissor) {
            id = i;
            break;
        }
    }

    if (id == -1) {
        return;
    }

    snprintf(explicacao, sizeof(explicacao), "A%d terminou", id + 1);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, mem->apps[id].pc);

    mem->apps[id].estado = FINALIZADO;
    mem->apps[id].terminou = 1;
    mem->vivos--;

    if (mem->rodando == id) {
        mem->rodando = -1;
    }

    kill(mem->apps[id].pid, SIGCONT);

    if (mem->vivos > 0 && mem->rodando == -1) {
        escalonar_proximo(mem);
    }

    imprimir_tabela(mem);
}

static MemoriaCompartilhada *criar_memoria(const char *nome_memoria) {
    int fd;
    void *addr;

    fd = shm_open(nome_memoria, O_CREAT | O_RDWR, 0600);

    if (fd == -1) {
        erro("shm_open");
    }

    if (ftruncate(fd, sizeof(MemoriaCompartilhada)) == -1) {
        erro("ftruncate");
    }

    addr = mmap(NULL,
                sizeof(MemoriaCompartilhada),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                0);

    if (addr == MAP_FAILED) {
        erro("mmap");
    }

    close(fd);

    memset(addr, 0, sizeof(MemoriaCompartilhada));

    return (MemoriaCompartilhada *)addr;
}

static void criar_app(MemoriaCompartilhada *mem,
                      const char *nome_memoria,
                      int id) {
    pid_t pid;
    char id_txt[16];
    char pid_kernel_txt[32];
    char explicacao[128];

    pid = fork();

    if (pid == -1) {
        erro("fork app");
    }

    if (pid == 0) {
        snprintf(id_txt, sizeof(id_txt), "%d", id);
        snprintf(pid_kernel_txt, sizeof(pid_kernel_txt), "%d", mem->pid_kernel);

        execl("./app",
              "app",
              id_txt,
              nome_memoria,
              pid_kernel_txt,
              NULL);

        erro("execl app");
    }

    mem->apps[id].pid = pid;
    mem->apps[id].id = id;
    mem->apps[id].pc = 0;
    mem->apps[id].estado = PRONTO;
    mem->apps[id].terminou = 0;
    mem->apps[id].syscall_pc = 0;
    mem->apps[id].syscall_op = 0;

    colocar_pronto(mem, id);

    snprintf(explicacao, sizeof(explicacao), "criou A%d e colocou na fila de prontos. PID=%d", id + 1, pid);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, 0);
}

static void criar_intercontroller(MemoriaCompartilhada *mem,
                                  const char *nome_memoria) {
    pid_t pid;
    char pid_kernel_txt[32];
    char explicacao[128];

    pid = fork();

    if (pid == -1) {
        erro("fork intercontroller");
    }

    if (pid == 0) {
        snprintf(pid_kernel_txt, sizeof(pid_kernel_txt), "%d", mem->pid_kernel);

        execl("./intercontroller",
              "intercontroller",
              nome_memoria,
              pid_kernel_txt,
              NULL);

        erro("execl intercontroller");
    }

    mem->pid_intercontroller = pid;

    snprintf(explicacao, sizeof(explicacao), "criou InterControllerSim. PID=%d", pid);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, -1);
}

int main(int argc, char **argv) {
    int n_apps = 3;
    int i;
    char nome_memoria[64];
    char explicacao[128];

    MemoriaCompartilhada *mem;

    sigset_t sinais;
    siginfo_t info;

    if (argc >= 2) {
        n_apps = atoi(argv[1]);
    }

    if (n_apps < MIN_APPS || n_apps > MAX_APPS) {
        printf("Uso: %s <quantidade_de_apps_3_a_6>\n", argv[0]);
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    snprintf(nome_memoria, sizeof(nome_memoria), "/trab1_so_%d", getpid());

    mem = criar_memoria(nome_memoria);

    mem->pid_kernel = getpid();
    mem->n_apps = n_apps;
    mem->vivos = n_apps;
    mem->rodando = -1;
    mem->tempo_simulacao = 0;
    mem->syscall_app = -1;

    sigemptyset(&sinais);
    sigaddset(&sinais, SINAL_IRQ0);
    sigaddset(&sinais, SINAL_IRQ1);
    sigaddset(&sinais, SINAL_SYSCALL);
    sigaddset(&sinais, SINAL_FIM);

    if (sigprocmask(SIG_BLOCK, &sinais, NULL) == -1) {
        erro("sigprocmask");
    }

    imprimir_cabecalho_log();

    snprintf(explicacao, sizeof(explicacao), "KernelSim iniciado. PID=%d", getpid());
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, -1);

    snprintf(explicacao, sizeof(explicacao), "memoria compartilhada criada: %s", nome_memoria);
    log_evento(mem->tempo_simulacao, "KERNEL", explicacao, -1);

    for (i = 0; i < n_apps; i++) {
        criar_app(mem, nome_memoria, i);
        sleep(1);
    }

    criar_intercontroller(mem, nome_memoria);

    sleep(3);

    log_evento(mem->tempo_simulacao, "KERNEL", "comecando escalonamento Round Robin", -1);
    escalonar_proximo(mem);

    while (mem->vivos > 0) {
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

        log_evento(mem->tempo_simulacao, "KERNEL", "todos os processos de aplicacao terminaram", -1);
    log_evento(mem->tempo_simulacao, "KERNEL", "encerrando InterControllerSim", -1);

    mem->encerrar = 1;

    for (i = 0; i < n_apps; i++) {
        waitpid(mem->apps[i].pid, NULL, 0);
    }

    waitpid(mem->pid_intercontroller, NULL, 0);

    munmap(mem, sizeof(MemoriaCompartilhada));
    shm_unlink(nome_memoria);

    log_evento(0, "KERNEL", "fim da simulacao", -1);

    return 0;
}