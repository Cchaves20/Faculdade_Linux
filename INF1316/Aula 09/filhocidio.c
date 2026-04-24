#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;

    if (argc < 2) {
        printf("Uso: %s [a|b|c|d]\n", argv[0]);
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        switch (argv[1][0]) {
            case 'a':
                while (1) {
                }
                break;

            case 'b':
                sleep(3);
                break;

            case 'c': {
                char *args[] = {"sleep", "5", NULL};
                execvp("sleep", args);
                perror("execvp sleep 5");
                exit(1);
            }

            case 'd': {
                char *args[] = {"sleep", "15", NULL};
                execvp("sleep", args);
                perror("execvp sleep 15");
                exit(1);
            }

            default:
                printf("Opcao invalida. Use a, b, c ou d.\n");
                exit(1);
        }

        exit(0);
    } else {
        sleep(2);
        printf("Pai tentando matar filho...\n");
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        printf("Filho finalizado.\n");
    }

    return 0;
}