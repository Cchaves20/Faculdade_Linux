#ifndef SIM_VIRTUAL_H
#define SIM_VIRTUAL_H

#include <stdint.h>

/* Os quatro algoritmos de substituicao que o simulador conhece. */
typedef enum {
    LRU,
    NRU,
    RELOGIO,
    OTIMO
} Algoritmo;

/* Um quadro (frame) da memoria fisica. Guarda a pagina que esta nele e os
 * bits de controle pedidos no enunciado: R (referenciada), M (modificada)
 * e o instante do ultimo acesso (usado pelo LRU). */
typedef struct {
    long pagina;            /* pagina carregada no quadro, ou -1 se vazio */
    int  r;                 /* bit de referencia */
    int  m;                 /* bit de modificacao (pagina suja) */
    unsigned long ultimo;   /* instante do ultimo acesso */
} Quadro;

/* Uma linha do arquivo .log: o endereco acessado e 'R' ou 'W'. */
typedef struct {
    uint32_t addr;
    char     rw;
} Acesso;

#endif
