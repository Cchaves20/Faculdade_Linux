#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    pid_t f1, f2;
    time_t inicio;
    int vez = 1;

    f1 = fork();
    if (f1 < 0) {
        perror("fork");
        return 1;
    }

    if (f1 == 0) {
        execl("./loop", "loop", NULL);
        perror("execl filho 1");
        exit(1);
    }

    f2 = fork();
    if (f2 < 0) {
        perror("fork");
        kill(f1, SIGKILL);
        waitpid(f1, NULL, 0);
        return 1;
    }

    if (f2 == 0) {
        execl("./loop", "loop", NULL);
        perror("execl filho 2");
        exit(1);
    }

    kill(f1, SIGSTOP);
    kill(f2, SIGSTOP);

    inicio = time(NULL);

    while (time(NULL) - inicio < 15) {
        if (vez == 1) {
            printf("\nExecutando filho 1\n");
            kill(f1, SIGCONT);
            sleep(1);
            kill(f1, SIGSTOP);
            vez = 2;
        } else {
            printf("\nExecutando filho 2\n");
            kill(f2, SIGCONT);
            sleep(1);
            kill(f2, SIGSTOP);
            vez = 1;
        }
    }

    printf("\nEncerrando filhos...\n");
    kill(f1, SIGKILL);
    kill(f2, SIGKILL);

    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);

    printf("Finalizado.\n");

    return 0;
}