/*
Nome: Caio Faria Brito Martins Chaves - Matricula: 2410162
Nome: Lucas Hufnagel Gromann de Araujo Goes - Matricula: 2410845
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "sim-virtual.h"

/* De quantos em quantos acessos zeramos os bits R no NRU (simula o tick do
 * relogio do SO, que e quem limparia esses bits periodicamente). */
#define TICK_NRU 1000

/* Le o nome do algoritmo passado na linha de comando. Aceita maiusculas ou
 * minusculas. Devolve 0 se reconheceu, -1 caso contrario. */
static int le_algoritmo(const char *s, Algoritmo *alg) {
    char nome[16];
    int i;

    for (i = 0; s[i] && i < 15; i++)
        nome[i] = tolower((unsigned char) s[i]);
    nome[i] = '\0';

    if (!strcmp(nome, "lru"))
        *alg = LRU;
    else if (!strcmp(nome, "nru"))
        *alg = NRU;
    else if (!strcmp(nome, "relogio") || !strcmp(nome, "clock"))
        *alg = RELOGIO;
    else if (!strcmp(nome, "otimo") || !strcmp(nome, "optimal"))
        *alg = OTIMO;
    else
        return -1;

    return 0;
}

static const char *nome_do_algoritmo(Algoritmo alg) {
    if (alg == LRU)     return "LRU";
    if (alg == NRU)     return "NRU";
    if (alg == RELOGIO) return "Relogio";
    return "Otimo";
}

/* Le o arquivo .log inteiro para um vetor. Precisamos de tudo na memoria por
 * causa do algoritmo Otimo, que precisa olhar os acessos futuros. */
static Acesso *carrega_log(const char *caminho, size_t *qtd) {
    FILE *fp = fopen(caminho, "r");
    if (!fp) {
        perror(caminho);
        exit(1);
    }

    size_t cap = 65536, n = 0;
    Acesso *v = malloc(cap * sizeof(Acesso));
    if (!v) {
        fprintf(stderr, "Sem memoria.\n");
        exit(1);
    }

    unsigned int addr;
    char rw;
    while (fscanf(fp, "%x %c", &addr, &rw) == 2) {
        if (n == cap) {
            cap *= 2;
            v = realloc(v, cap * sizeof(Acesso));
            if (!v) {
                fprintf(stderr, "Sem memoria.\n");
                exit(1);
            }
        }
        v[n].addr = addr;
        v[n].rw   = toupper((unsigned char) rw);
        n++;
    }

    fclose(fp);
    *qtd = n;
    return v;
}

/* LRU: tira a pagina usada ha mais tempo (menor "ultimo"). */
static int vitima_lru(Quadro *q, int nq) {
    int v = 0;
    for (int i = 1; i < nq; i++)
        if (q[i].ultimo < q[v].ultimo)
            v = i;
    return v;
}

/* NRU: classes a partir de (R, M) -> 0:(0,0) 1:(0,1) 2:(1,0) 3:(1,1).
 * Escolhe uma pagina da menor classe que existir. */
static int vitima_nru(Quadro *q, int nq) {
    int melhor = -1, classe_melhor = 4;
    for (int i = 0; i < nq; i++) {
        int c = (q[i].r << 1) | q[i].m;
        if (c < classe_melhor) {
            classe_melhor = c;
            melhor = i;
            if (c == 0)
                break;          /* nao tem como achar classe menor */
        }
    }
    return melhor;
}

/* Relogio (segunda chance): anda em circulo; se R==1 da mais uma chance e
 * zera R, se R==0 essa e a vitima. O ponteiro fica guardado entre chamadas. */
static int vitima_relogio(Quadro *q, int nq, int *ponteiro) {
    while (q[*ponteiro].r) {
        q[*ponteiro].r = 0;
        *ponteiro = (*ponteiro + 1) % nq;
    }
    int v = *ponteiro;
    *ponteiro = (*ponteiro + 1) % nq;
    return v;
}

/* Otimo: tira a pagina cujo proximo uso esta mais longe no futuro. O proximo
 * uso de cada pagina sai direto dos vetores de ocorrencias (ver main da
 * simulacao), entao isso aqui e so achar o maximo. */
static int vitima_otimo(Quadro *q, int nq, const size_t *flat,
                        const size_t *pos, const size_t *fim, size_t total) {
    int v = 0;
    size_t mais_longe = 0;
    for (int i = 0; i < nq; i++) {
        long p = q[i].pagina;
        size_t prox = (pos[p] < fim[p]) ? flat[pos[p]] : total;
        if (prox > mais_longe) {
            mais_longe = prox;
            v = i;
            if (prox == total)
                break;          /* "nunca mais usada" e o mais longe possivel */
        }
    }
    return v;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
            "uso: %s <LRU|NRU|Relogio|Otimo> <arquivo.log> <pagina_KB> <memoria_MB>\n",
            argv[0]);
        return 1;
    }

    Algoritmo alg;
    if (le_algoritmo(argv[1], &alg) != 0) {
        fprintf(stderr, "Algoritmo invalido: %s\n", argv[1]);
        return 1;
    }

    const char *arquivo = argv[2];
    int pagina_kb  = atoi(argv[3]);
    int memoria_mb = atoi(argv[4]);
    if (pagina_kb <= 0 || memoria_mb <= 0) {
        fprintf(stderr, "Pagina e memoria precisam ser positivos.\n");
        return 1;
    }

    /* Quantos bits do endereco sao deslocamento dentro da pagina. Como a
     * pagina e potencia de 2, basta contar quantas vezes da pra dividir por 2.
     * Ex.: 4KB = 4096 -> 12 bits; 8KB = 8192 -> 13 bits. */
    unsigned int tam_pagina = (unsigned int) pagina_kb * 1024;
    int s = 0;
    for (unsigned int t = tam_pagina; t > 1; t >>= 1)
        s++;

    int num_quadros = (memoria_mb * 1024 * 1024) / tam_pagina;
    long num_paginas = 1L << (32 - s);   /* tamanho da tabela de paginas */

    size_t n;
    Acesso *acessos = carrega_log(arquivo, &n);

    /* Tabela de paginas: dado o numero da pagina, diz em que quadro ela esta
     * (ou -1). Ocupa bastante memoria, mas o enunciado diz que tudo bem pra
     * simulacao de um programa so. */
    int *tabela = malloc(num_paginas * sizeof(int));
    Quadro *quadros = malloc(num_quadros * sizeof(Quadro));
    if (!tabela || !quadros) {
        fprintf(stderr, "Sem memoria.\n");
        return 1;
    }
    for (long i = 0; i < num_paginas; i++)
        tabela[i] = -1;
    for (int i = 0; i < num_quadros; i++) {
        quadros[i].pagina = -1;
        quadros[i].r = 0;
        quadros[i].m = 0;
        quadros[i].ultimo = 0;
    }

    /* So o Otimo usa estes vetores. A ideia: para cada pagina guardamos, em
     * ordem, os instantes em que ela e acessada (tudo num vetor unico "flat",
     * com inicio/fim de cada pagina). Assim da pra saber o proximo uso em O(1).
     * "pos" e a posicao atual dentro do bloco da pagina. */
    size_t *flat = NULL, *ini = NULL, *fim = NULL, *pos = NULL;
    if (alg == OTIMO) {
        ini = calloc(num_paginas, sizeof(size_t));
        fim = calloc(num_paginas, sizeof(size_t));
        pos = malloc(num_paginas * sizeof(size_t));
        flat = malloc((n ? n : 1) * sizeof(size_t));
        if (!ini || !fim || !pos || !flat) {
            fprintf(stderr, "Sem memoria.\n");
            return 1;
        }
        /* primeiro conta quantos acessos cada pagina tem... */
        for (size_t i = 0; i < n; i++)
            fim[acessos[i].addr >> s]++;
        /* ...transforma em offsets (inicio e fim de cada bloco)... */
        size_t soma = 0;
        for (long p = 0; p < num_paginas; p++) {
            ini[p] = soma;
            soma += fim[p];
            fim[p] = soma;
            pos[p] = ini[p];
        }
        /* ...e preenche o vetor flat com os instantes de cada acesso. */
        for (size_t i = 0; i < n; i++)
            flat[pos[acessos[i].addr >> s]++] = i;
        for (long p = 0; p < num_paginas; p++)
            pos[p] = ini[p];
    }

    unsigned long page_faults = 0;
    unsigned long escritas = 0;
    int ocupados = 0;      /* quadros ja preenchidos */
    int ponteiro = 0;      /* ponteiro do algoritmo do relogio */

    for (size_t tempo = 0; tempo < n; tempo++) {
        long pagina = acessos[tempo].addr >> s;
        int  escrita = (acessos[tempo].rw == 'W');

        /* No Otimo, avanca o ponteiro de ocorrencias desta pagina pra que ele
         * passe a apontar pro proximo uso dela. */
        if (alg == OTIMO)
            pos[pagina]++;

        /* No NRU, de tempos em tempos limpamos os bits R. */
        if (alg == NRU && tempo % TICK_NRU == 0)
            for (int i = 0; i < ocupados; i++)
                quadros[i].r = 0;

        int quadro = tabela[pagina];
        if (quadro != -1) {
            /* a pagina ja esta na memoria: so atualiza os bits */
            quadros[quadro].r = 1;
            quadros[quadro].ultimo = tempo;
            if (escrita)
                quadros[quadro].m = 1;
            continue;
        }

        /* deu page fault: precisa trazer a pagina */
        page_faults++;

        int destino;
        if (ocupados < num_quadros) {
            destino = ocupados++;        /* ainda tem quadro livre */
        } else {
            /* memoria cheia: escolhe quem sai */
            switch (alg) {
                case LRU:     destino = vitima_lru(quadros, num_quadros); break;
                case NRU:     destino = vitima_nru(quadros, num_quadros); break;
                case RELOGIO: destino = vitima_relogio(quadros, num_quadros, &ponteiro); break;
                default:      destino = vitima_otimo(quadros, num_quadros, flat, pos, fim, n); break;
            }
            /* se a pagina que vai sair estava suja, ela seria escrita no disco */
            if (quadros[destino].m)
                escritas++;
            tabela[quadros[destino].pagina] = -1;
        }

        quadros[destino].pagina = pagina;
        quadros[destino].r = 1;
        quadros[destino].m = escrita ? 1 : 0;
        quadros[destino].ultimo = tempo;
        tabela[pagina] = destino;
    }

    printf("Executando o simulador...\n");
    printf("Arquivo de entrada: %s\n", arquivo);
    printf("Tamanho da memoria fisica: %d MB\n", memoria_mb);
    printf("Tamanho das paginas: %d KB\n", pagina_kb);
    printf("Algoritmo de substituicao: %s\n", nome_do_algoritmo(alg));
    printf("Numero de Faltas de Paginas: %lu\n", page_faults);
    printf("Numero de Paginas Escritas: %lu\n", escritas);

    free(acessos);
    free(tabela);
    free(quadros);
    free(flat);
    free(ini);
    free(fim);
    free(pos);
    return 0;
}
