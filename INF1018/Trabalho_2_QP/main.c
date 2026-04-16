#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "gera_codigo.h"

#define PAGE_START(P) ((intptr_t)(P)&~(pagesize-1))
#define PAGE_END(P) (((intptr_t)(P)+pagesize-1)&~(pagesize-1))

unsigned char codigo[1000];

int execpage(void *ptr, size_t len) {
	int ret;

	const long pagesize = sysconf(_SC_PAGE_SIZE);
	if (pagesize == -1)
		return -1;

	ret = mprotect((void *)PAGE_START(ptr),
		 PAGE_END((intptr_t)ptr + len) - PAGE_START(ptr),
		 PROT_READ | PROT_WRITE | PROT_EXEC);
	if (ret == -1)
		return -1;

	return 0;
}

#undef PAGE_START
#undef PAGE_END

/*
 * Programa de teste para o gerador de código LBS
 * Usa convenção x86-64 conforme o enunciado.
 */

int main(int argc, char *argv[]) {
    FILE *fp;
    funcp funcLBS;
    unsigned char code[500];
    int res;

    if (argc < 3) {
        printf("Uso: %s <arquivo_LBS> <valor>\n", argv[0]);
        return 1;
    }

    /* abre o arquivo LBS */
    fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    /* gera o código de máquina */
    gera_codigo(fp, code, &funcLBS);
    fclose(fp);

    if (funcLBS == NULL) {
        printf("Erro na geração do código\n");
        return 1;
    }

    /* executa a função gerada */
    int arg = atoi(argv[2]);
    res = (*funcLBS)(arg);

    printf("Resultado da execução: %d\n", res);

    return 0;
}
