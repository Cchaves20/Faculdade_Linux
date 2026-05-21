#include "common.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Uso interno: intercontroller <nome_memoria> <pid_kernel>\n");
        return 1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    const char *nome_memoria = argv[1];
    pid_t pid_kernel = (pid_t)atoi(argv[2]);

    MemoriaCompartilhada *mem = abrir_memoria(nome_memoria);

    int tempo_io = 0;

    log_evento(mem->tempo_simulacao,
               "IC",
               "InterControllerSim iniciado",
               -1);

    while (!mem->encerrar) {
        sleep(TIME_SLICE);

        if (mem->encerrar) {
            break;
        }

        mem->tempo_simulacao++;

        log_evento(mem->tempo_simulacao,
                   "IC",
                   "gerou IRQ0 por fim de time-slice",
                   -1);

        kill(pid_kernel, SINAL_IRQ0);

        if (mem->bloqueados_qtd > 0) {
            tempo_io++;

            if (tempo_io == TEMPO_IO) {
                log_evento(mem->tempo_simulacao,
                           "IC",
                           "gerou IRQ1 por fim de E/S em D1",
                           -1);

                kill(pid_kernel, SINAL_IRQ1);

                tempo_io = 0;
            }
        } else {
            tempo_io = 0;
        }
    }

    log_evento(mem->tempo_simulacao,
               "IC",
               "encerrando InterControllerSim",
               -1);

    munmap(mem, sizeof(MemoriaCompartilhada));

    return 0;
}