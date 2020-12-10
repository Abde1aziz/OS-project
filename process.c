#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int argc, char *argv[])
{
    initClk();


int runningTime= atoi(argv[1]);
remainingtime=runningTime;
printf("Hi I am process %d, My running time is %d, time now is %d\n ",getpid(),runningTime,getClk());
    
    while (remainingtime > 0)
    {
       remainingtime = runningTime- (int)clock()/CLOCKS_PER_SEC;
    }
    printf("Hi I am process %d, Iam done at %d\n",getpid(),getClk());
    destroyClk(false);
    
    return 0;
}
