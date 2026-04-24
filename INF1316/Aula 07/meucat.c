#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char ch;

    if (argc < 2) 
    {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) 
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    while ((ch = fgetc(file)) != EOF) 
    {
        putchar(ch);
    }

    fclose(file);
    return 0;
}
