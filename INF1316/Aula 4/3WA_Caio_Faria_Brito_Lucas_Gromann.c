#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void) 
{
    int n = 1;
    int status = 1;

    if (fork() != 0)
    {
        /* codigo pai */

        for (int i = 0; i < 1000; i++)
        {
            n++;
            printf("processo pai, pid = %d ,n = %d\n", getpid(), n);
        }

        waitpid(-1, &status, 0);
    }
    else
    {
        if (fork() != 0)
        {
            /* codigo filho */

            for (int i = 0; i < 1000; i++)
            {
                n += 10;
                printf("processo filho, pid = %d ,n = %d\n", getpid(), n);
            }

            waitpid(-1, &status, 0);
        }
        else
        {
            /* codigo neto */
            
            for (int i = 0; i < 1000; i++)
            {
                n += 100;
                printf("processo neto, pid = %d ,n = %d\n", getpid(), n);
            }
        }
    }
    exit(1);
}