#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int remainingtime = atoi(argv[1]);
    printf("process runtime is %d\n", remainingtime);

    while (remainingtime > 0)
    {
        sleep(1);
        remainingtime--;
    }

    printf("Process %d has finished at time %d\n", getpid(), getClk());

    kill(getppid(), SIGCHLD);
    destroyClk(false);

    return 0;
}