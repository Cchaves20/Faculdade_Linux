#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
void type_prompt(void)
{
    printf("$");
}

int main(void)
{
    char vTemp[MAX_LINE];
    char* vStrArgs[MAX_ARGS];
    int status = 1;
    while (1)
    {
        type_prompt();

        int i = 0;
        while(i < MAX_ARGS -1)
        {
            if(scanf("%s", vTemp) != 1)
            {
                break;
            }
            vStrArgs[i] = malloc(strlen(vTemp) + 1);
            strcpy(vStrArgs[i], vTemp);
            i++;

            char c = getchar();
            if (c == '\n' || c == EOF) break;
        }
        
        vStrArgs[i] = NULL;

        if(strcmp(vStrArgs[0], "exit") == 0) break;

        if(fork() != 0)
        {
            waitpid(-1, &status, 0);
            for(int j = 0; j < i; j++)
            {
                free(vStrArgs[j]);
            }
        }
        else
        {
            if (execvp(vStrArgs[0], vStrArgs) == -1) 
            {
                printf("Erro");
                exit(1);
            }
        }
    }
    return 0;
}