#include <stdio.h>
#include <unistd.h>

int main() {
    while (1) {
        printf("Processo PID %d executando\n", getpid());
        sleep(1);
    }
    return 0;
}