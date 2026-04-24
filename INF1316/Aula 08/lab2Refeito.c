#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(void) {

    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    int *n = (int *) shmat(shmid, NULL, 0);

    *n = 1;

    pid_t pid;

    // Filho 1
    pid = fork();
    if (pid == 0) {
        for (int j = 0; j < 1000; j++) {
            (*n) += 1;
        }
        printf("Filho 1 (pid=%d), n=%d\n", getpid(), *n);
        exit(0);
    }

    // Filho 2
    pid = fork();
    if (pid == 0) {
        for (int j = 0; j < 1000; j++) {
            (*n) += 10;
        }
        printf("Filho 2 (pid=%d), n=%d\n", getpid(), *n);
        exit(0);
    }

    // Filho 3
    pid = fork();
    if (pid == 0) {
        for (int j = 0; j < 1000; j++) {
            (*n) += 100;
        }
        printf("Filho 3 (pid=%d), n=%d\n", getpid(), *n);
        exit(0);
    }

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    printf("Valor final de n = %d\n", *n);

    shmdt(n);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}