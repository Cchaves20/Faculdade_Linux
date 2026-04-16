/*João Paulo Frighetto 2410207 3WB*/
/*Caio Faria Brito 2410162 3WB*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bytes_necessarios_unsigned(unsigned int valor) {
    if (valor <= 0xFF) return 1;
    if (valor <= 0xFFFF) return 2;
    if (valor <= 0xFFFFFF) return 3;
    return 4;
    /*aqui verifico a quantidade de bytes que os unsigned ints utilizam, para, mais tarde, remover os bytes não utilizados do arquivo binario. Faço isso checando se o valor do int é menor ou igual ao valor equivalente a o maior valor por byte possivel (por ex: se for <=FF (maior valor possivel que cabe em 1 byte), retorna 1). é possivel fazer essa função dessa forma, pois o valor é unsigned, o que significa que não é necessário se preocupar com sinal e, portanto, não tem confusão entre negativo e positivo.*/
}

int bytes_necessarios_signed(int valor) {
    if (valor >= -128 && valor <= 127) return 1;
    if (valor >= -32768 && valor <= 32767) return 2;
    if (valor >= -8388608 && valor <= 8388607) return 3;
    return 4;
    /*aqui verifico a quantidade de bytes que os signed ints utilizam, para, mais tarde, remover os bytes não utilizados do arquivo binario. Faço isso checando se o valor do int é menor ou igual ao valor equivalente a o maior e o menor valor possivel por byte (por ex: se for <=127 (maior valor possivel que cabe em 1 byte) ou >=-128 (menor valor possivel), retorna 1). decidi fazer desta forma e não equivalente à como eu fiz com o unsigned int por conta dos números negativos, uma vez que, ao irrelevar o sinal, pode trocar o numero e, dessa forma, dar algum tipo de erro. |Prevendo isso, decidi me dar esse trabalho extra (apesar de que agora com a função já feita, não sei se era*/
}

void escreve_int(FILE *arquivo, int valor, int nbytes) {
    for (int i = nbytes - 1; i >= 0; i--) {
        fputc((valor >> (i * 8)), arquivo);
    }
    /*aqui eu escrevo o valor do int no arquivo de bits a partir do fputc*/
}

void escreve_uint(FILE *arquivo, unsigned int valor, int nbytes) {
    for (int i = nbytes - 1; i >= 0; i--) {
        fputc((valor >> (i * 8)), arquivo);
    }
    /*aqui eu escrevo o valor do unsigned int int no arquivo de bits a partir do fputc*/
}

int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
    unsigned char *p = (unsigned char *)valores;
    /*aqui eu crio uma variavel que tem o mesmo valor do valores(void) mas em um unsigned char. Faço isso para facilitar/habilitar a manipulação bit a bit*/
    
    fwrite(&nstructs, 1, 1, arquivo);
    /*aqui eu escrevo o numero de structs que tem no vetor*/

    while (nstructs--) {
      /*aqui eu inicio um while que continua enquanto houver structs*/ 
        char *d = descritor;
        /*aqui eu crio uma variavel com o mesmo valor da string descritora. Faço isso pois, caso eu utilizasse o próprio descritor, a partir da segunda struct eu não conseguiria ler novamente, quebrando o código inteiro*/
        int ultimo = 0;
        /*aqui eu crio um int para o ultimo valor. faço isso para facilitar trocar seu valor*/
        while (*d) {
            if (*(d+1) == '\0' || (*(d+3) == '\0' && *d == 's')) ultimo = 1;
            /*aqui eu altero o valor do ultimo caso o prox valor seja o ultimo caracter da string*/

            if (*d == 'i') {
              /*aqui vejo se o ponteiro d e um int com sinal*/
                while(((size_t)p)%4 != 0){
                  p++;
                }
                /*aqui garanto o alinhamento de 4 bytes*/
                int *pi = (int *)p;
                int nbytes = bytes_necessarios_signed(*pi);
                unsigned char cabecalho = (ultimo << 7) | (0x1 << 5) | (nbytes & 0x1F);
                /*aqui crio o cabeçalho: 1 bit para último, 2 bits para tipo (01 = signed), 5 bits para tamanho*/
                if (fputc(cabecalho, arquivo) == EOF) return -1;
                escreve_int(arquivo, *pi, nbytes);
                p += sizeof(int);
                d += 1;
            } else if (*d == 'u') {
                /*aqui vejo se o ponteiro d e um int sem sinal*/
                while(sizeof(p)%4 != 0){
                  p++;
                }
                /* aqui garanto o alinhamento de 4 bytes*/
                unsigned int *pu = (unsigned int *)p;
                int nbytes = bytes_necessarios_unsigned(*pu);
                unsigned char cabecalho = (ultimo << 7) | (0x0 << 5) | (nbytes & 0x1F);
                 /*aqui crio o cabeçalho: 1 bit para último, 2 bits para tipo (00 = unsigned), 5 bits para tamanho*/
                if (fputc(cabecalho, arquivo) == EOF) return -1;
                escreve_uint(arquivo, *pu, nbytes);
                p += sizeof(unsigned int);  
                d += 1;
            } else if (*d == 's') {
              /*aqui vejo se o ponteiro d e uma string*/
                int tam = (d[1] - '0') * 10 + (d[2] - '0');
                /*aqui extraio o tamanho fixo da string do descritor*/
                char *str = (char *)p;
                int len = strlen(str);
                /* aqui calculo o comprimento real da string */
                unsigned char cabecalho = (ultimo << 7) | (1 << 6) | (len & 0x3F);
                /*aqui crio o cabeçalho: 1 bit para último, 1 bit para indicar string, 6 bits para comprimento*/

                if (fputc(cabecalho, arquivo) == EOF) return -1;
                if (fwrite(str, len, 1, arquivo) ==  0) return -1;
                /* aqui escrevo a string no arquivo (sem \0) */
                p += tam;
                d += 3;
            }
        }
    }

    return 0;
}

int le_int(FILE *arquivo, int nbytes) {
    int valor = 0;
    for (int i = 0; i < nbytes; i++) {
        int c = fgetc(arquivo);
        if (c == EOF) return 0;
        valor = (valor << 8) | (c & 0xFF);
    }
    if (nbytes < 4 && (valor & (1 << ((nbytes * 8) - 1)))) {
        valor |= -1 << (nbytes * 8);
    }
    return valor;
    /*aqui eu leio um inteiro com sinal a partir do arquivo, com a quantidade de bytes informada, tambem faco a extensão de sinal se o número não tiver os 4 bytes completos.*/
}

unsigned int le_uint(FILE *arquivo, int nbytes) {
    unsigned int valor = 0;
    for (int i = 0; i < nbytes; i++) {
        int c = fgetc(arquivo);
        if (c == EOF) return 0;
        valor = (valor << 8) | (c & 0xFF);
    }
    return valor;
    /*aqui leio um inteiro sem sinal com n bytes de tamanho do arquivo*/
}

void mostracomp(FILE *arquivo) {
    int nstructs = fgetc(arquivo);
    printf("Estruturas: %d\n\n", nstructs);
    /*aqui leio e exibo o número de estruturas presentes no arquivo*/
    for (int i = 0; i < nstructs; i++) {
        int fim = 0;
        while (!fim) {
            int cabecalho = fgetc(arquivo);
            if (cabecalho == EOF) return;

            int cont = (cabecalho >> 7) & 0x1;
            int tipo = (cabecalho >> 5) & 0x3;
            /* aqui extraio os bits do cabeçalho: bit 7 é se é o último campo, bits 6-5 indicam o tipo*/

            if (tipo == 0x0) { // unsigned
                int nbytes = cabecalho & 0x1F;
                unsigned int valor = le_uint(arquivo, nbytes);
                printf("(uns) %u (%08x)\n", valor, valor);
            } else if (tipo == 0x1) { // signed
                int nbytes = cabecalho & 0x1F;
                int valor = le_int(arquivo, nbytes);
                printf("(int) %d (%08x)\n", valor, valor);
            } else { // string
                int len = cabecalho & 0x3F;
                char str[64];
                memset(str, 0, sizeof(str));
                fread(str, 1, len, arquivo);
                str[len] = '\0';
                printf("(str) %s\n", str);
            }

            if (cont) {
                printf("\n");
                fim = 1;
            }
        }
    }
}


