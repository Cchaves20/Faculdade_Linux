#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TRUE 1
#define MAX_LINE 1024
#define MAX_ARGS 64

void type_prompt() {
    printf("myshell> ");
    fflush(stdout);
}

void read_command(char *command, char **parameters) {
    char line[MAX_LINE];
    
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        exit(0);
    }

    line[strcspn(line, "\n")] = 0;

    int i = 0;
    char *token = strtok(line, " ");

    while (token != NULL && i < MAX_ARGS - 1) {
        parameters[i] = token;
        token = strtok(NULL, " ");
        i++;
    }

    parameters[i] = NULL;

    if (parameters[0] != NULL) {
        strcpy(command, parameters[0]);
    }
}

int main() {

    char command[MAX_LINE];
    char *parameters[MAX_ARGS];
    int status;

    while (TRUE) {

        type_prompt();
        read_command(command, parameters);

        if (parameters[0] == NULL)
            continue;

        if (strcmp(command, "exit") == 0)
            break;

        if (fork() != 0) {
            waitpid(-1, &status, 0);
        } else {
            execvp(command, parameters);
            perror("exec failed");
            exit(1);
        }
    }

    return 0;
}
