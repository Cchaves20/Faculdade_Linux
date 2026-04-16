// teste.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gravacomp.h"

struct s {
    int i;
    char s1[5];
    unsigned int u;
};

int main() {
    struct s exemplo[2] = {
        { -2, "abcd", 258 },
        {  2, "ABCD", 65535 }
    };

    FILE *arq = fopen("saida.bin", "wb");
    if (!arq) {
        printf("Erro abrindo arquivo");
    }
      
    if (gravacomp(2, exemplo, "is04u", arq) == -1) {
        printf("Erro na gravação\n");
        return -1;
    }
    fclose(arq);

    arq = fopen("saida.bin", "rb");
    if (!arq) {
        perror("Erro abrindo arquivo para leitura");
        return 1;
    }

    mostracomp(arq);
    fclose(arq);

    return 0;
}
