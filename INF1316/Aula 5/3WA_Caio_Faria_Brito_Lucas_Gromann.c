#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void) 
{
    int n = 1;
    int status = 1;

    int filho1 = fork();
    int filho2 = fork();
    int filho3 = fork();
    
    /* codigo filho 1*/
    if (filho1)
    { 
        for (int j = 0; j < 1000; j++)
        {
            n += 1;
            printf("processo filho 1, pid = %d ,n = %d\n", getpid(), n);
        }
        printf("soma total: %d\n", n);
        exit(1);
    }
    /* codigo filho 2*/
    if (filho2)
    { 
        for (int j = 0; j < 1000; j++)
        {
            n += 10;
            printf("processo filho 2, pid = %d ,n = %d\n", getpid(), n);
        }
        printf("soma total: %d\n", n);
        exit(1);
    }
    /* codigo filho 3*/
    if (filho3)
    { 
        for (int j = 0; j < 1000; j++)
        {
            n += 100;
            printf("processo filho 3, pid = %d ,n = %d\n", getpid(), n);
        }
        printf("soma total: %d\n", n);
        exit(1);
    }
    exit(1);
}