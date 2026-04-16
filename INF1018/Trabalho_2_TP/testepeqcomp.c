/*Nome: Caio Faria Brito Martins Chaves*/
/*Matricula: 2410162*/
/*Nome: Joao Paulo de Araujo Frighetto*/
/*Matricula: 2410207*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "peqcomp.h"

/*main*/
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

int main(int argc, char *argv[])
{
    execpage(codigo, sizeof codigo);
    FILE* arq = fopen(argv[1],"r");
    if (arq == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
        exit(1);
    }
    funcp variavel = peqcomp(arq, codigo);
    int resultado = variavel();
    printf("%d\n", resultado);
    return 0;
}