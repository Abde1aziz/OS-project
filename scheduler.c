
#include "headers.h"
#include "queue.h"
#include "priority_queue.h"
#include <string.h>


pid_t pid;
key_t msgqid1;
//key_t msgqid2;
FILE * logFilePtr;
bool available = true;
pData runningProcess;
pData currentProc;
int WaitTime;
int numberOfFinishedProcesses = 0;

void hpfHandler(int signum);
void clearResources(int signum);

void  HPF() {
    //initializing some staff
    printf("HPF Scheduler Initialized\n");
    bool firstIteration = 1;
    priority_heap_t *Q = (priority_heap_t*)calloc(1,sizeof(priority_heap_t));
    printf("Priority Queu created\n");

    //Beginning of the Sceduler logic
    while(1) {
        //initializing the priority Q parameters
        if(firstIteration) {
            initializePQueue(Q);
            printf("Priority Q initialized\n");
            firstIteration = 0;
        }

        //Creating a container for the recieved process
        struct Data recievedProcess;
        pData recievedData;
        //Recieving Processes that are in the Message Queue
        while(1) {
            size_t rcv = msgrcv(msgqid1, &recievedProcess, sizeof(recievedProcess), 0, IPC_NOWAIT);
            if (rcv == -1) {
                //perror("Error in recieve\n");
                break;
            }
            else {
                printf("Message Recieved\n");
                recievedData.arrival = recievedProcess.arrival;
                recievedData.pid = recievedProcess.id;
                recievedData.priority = recievedProcess.priority;
                recievedData.runtime = recievedProcess.runtime;
                printf("\n %d %d\n",recievedData.arrival,recievedData.runtime );

                //Forking  processes corresponding to the new Data recived
                int cpid = fork();
                if(cpid == -1) {
                    printf("Error in fork..\n");
                }
                else if(cpid == 0) {
                    //Passing the runtime to the child proccess
                    char buf[3];
                    sprintf(buf, "%d", recievedData.runtime);
                    char *argv[] = { "./process.out", buf, 0};
                    printf("process %d forked\n", getpid());
                    execve(argv[0], &argv[0], NULL);
                }
                else {
                    //Adding the new processes received to the ready Queue
                    recievedData.pid = cpid;
                    recievedData.isRunning = false;
                    push(Q, recievedData);
                    kill(cpid, SIGSTOP);
                    printf("Process %d is in ready state\n", cpid);
                    if(recievedData.priority > runningProcess.priority && !available && !firstIteration){
                        kill(runningProcess.pid, SIGSTOP);
                        runningProcess.isRunning = false;
                        push(Q, runningProcess);
                        printf("process %d has stopped because there is another one with higher priority arrived", runningProcess.pid);
                        available = true;
                    }
                }
            }
        }
        //If the processor is not busy and there is ready processes set the appropriate process to active
        if (Q->len != 0 && available == true) {
            runningProcess = pop(Q);
            runningProcess.isRunning = 1;
            printf("Process %d is now active at time %d\n", runningProcess.pid, getClk());
            WaitTime = getClk() - runningProcess.arrival;
            fprintf(logFilePtr, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %0.2f\n",
                                getClk(),runningProcess.pid, runningProcess.arrival, runningProcess.runtime, 0, WaitTime, (getClk() - runningProcess.arrival) /*TA*/, (getClk() - runningProcess.arrival)/ runningProcess.runtime /*WTA*/); 
            kill(runningProcess.pid, SIGCONT);
            available = false;
        }

        sleep(1);

    }
    //upon termination release the clock resources
    destroyClk(true);
}

void SRTN(){
    printf("entered SRTN algorithm\n" );
    bool firstIteration = 1;
    long processInterTime;
    priority_heap_t *Q = (priority_heap_t*)calloc(1,sizeof(priority_heap_t));
    printf("Priority Queu created\n");
    while(1){
        if(firstIteration){
            initializePQueue(Q);
            printf("Priority Q initialized\n");
            pid = getpid();
            firstIteration = 0;
        }
        struct Data recievedProcess;
        pData recievedData;
        //Recieving Processes that are in the Message Queue
        while(1){
            size_t rcv = msgrcv(msgqid1, &recievedProcess, sizeof(recievedProcess), 0, IPC_NOWAIT);
            if (rcv == -1) {
                // //perror("Error in recieve\n");
                // break;
            }
            else {
                printf("Message Recieved\n");
                recievedData.arrival = recievedProcess.arrival;
                recievedData.pid = recievedProcess.id;
                recievedData.priority = recievedProcess.priority;
                recievedData.runtime = recievedProcess.runtime;
                //fork a process for the comming data
                printf("\n %d %d\n",recievedData.arrival,recievedData.runtime );
                int cpid = fork();
                if(cpid == -1) {
                    printf("Error in fork..\n");
                }
                else if(cpid == 0) {
                char buf[3];
                // itoa(currentProcessData.runtime, buf, 10);
                sprintf(buf, "%d", recievedData.runtime);
                char *argv[] = { "./process.out", buf, 0};
                execve(argv[0], &argv[0], NULL);
                }
                else {
                    recievedData.pid = cpid;
                    recievedData.isRunning = false;
                     //push(Q, recievedData);
                    printf("Process added %d to Q\n", cpid);
                    kill(cpid, SIGSTOP);
                    printf("Process %d is in ready state\n", cpid);

                    if (available == true)
                    {
                        printf("there was no processes running so this process started\n" );
                        runningProcess.pid = recievedData.pid;
                        runningProcess.runtime = recievedData.runtime;
                        //  printf("\n data id %d\n",cpid );
                        runningProcess.isRunning = 1;
                        available = false;
                        processInterTime = getClk();
                        printf("process inter time %d\n",processInterTime);
                        kill(runningProcess.pid,SIGCONT);
                    }
                    else {
                        //if there is no process running run the current one
                        //calculate the time passed in the currentProc
                        runningProcess.remainingT = runningProcess.runtime - clock()/CLOCKS_PER_SEC;
                        //if it is there one running compare between the running time
                        printf("runningProcess.remainingT %d\n",runningProcess.remainingT );
                        if(recievedData.remainingT < runningProcess.remainingT )
                        {
                            kill(runningProcess.pid,SIGSTOP);

                            //kill(recievedData.id,SIGCONT); //it is already running
                            //reaProc = insertSorted(reaProc,currentProc);
                            runningProcess.priority = runningProcess.remainingT;
                            push(Q,runningProcess);
                            // it will be more effiecient if you write copy constructor
                            runningProcess.pid = recievedData.pid;
                            runningProcess.arrival = recievedData.arrival;
                            runningProcess.priority = recievedData.priority;
                            runningProcess.remainingT = recievedData.remainingT;
                            runningProcess.runtime = recievedData.runtime;
                            processInterTime = getClk();
                            kill(recievedData.pid,SIGCONT);
                        }
                        else
                        {
                    // use insert sorted to insert the one you want in the ready queue
                    //reaProc = insertSorted(reaProc,recievedData);
                            recievedData.priority = recievedData.runtime;
                            push(Q,recievedData);
                        }
                    }
                } 
            }

            if (Q->len != 0 && available == true) {
                printf("Condition is true\n");
                runningProcess = pop(Q);
                runningProcess.isRunning = 1;
                printf("Process %d is now active\n", runningProcess.pid);
                fprintf(logFilePtr, "At time %d process %d started arr %d remain %d wait %d\n",
                    getClk(),runningProcess.pid, runningProcess.arrival, runningProcess.runtime, (runningProcess.arrival-getClk()));
                kill(runningProcess.pid, SIGCONT);
                available = false;
            }
        }   
        sleep(1); 
    }    
    destroyClk(true);
}    

 pData recv_msg_process(key_t msgqid)
{
    int recv_val;
     pData processData;

    // receive all messages
    recv_val = msgrcv(msgqid, &processData, sizeof(processData), 0, !IPC_NOWAIT);

    if (recv_val == -1)
    {
        perror("Error in receive, bad data returned");
        return processData;
    }
    else
    {
        printf("\nProcess Data received\n");
        return processData;
    }
}

// void RR(){

//         printf("Round Robin in action\n");
//         /////////////////////////////////////////////////////////////////
//         //////// receive the messeage
//         //signal(SIGCHLD, child_handler_Round_Robin);
//         struct Gen_to_Sch message;
//         int msgid;
//         msgid = msgget(1, 0666);
//         char remaining_char[7];
//         int Qunata = atoi(argv[2]);
//         printf("Quanta %d\n", Qunata);
//         // msgrcv to receive message
//         struct processData *temp;
//         head = NULL;
//         flag = 1;
//         pointer1 = head;
//         while (1)
//         {
//             int rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
//             // checking of the message recived
//             if (rcvTag != -1)
//             {
//                 ///// There are processes at the message queue, so inserting them in the priority queue
//                 ////// DO Stuff to to insert them in the priority queue
//                 pid_childs = fork();
//                 if (pid_childs == -1)
//                     printf("error in forking Processes");

//                 else if (pid_childs == 0)
//                 {
//                     char remaining_char[7];

//                     sprintf(remaining_char, "%d", message.ProcessData.runningtime);

//                     char *argv[] = {"./process.out", remaining_char, 0};
//                     execve(argv[0], &argv[0], NULL); //// forking the processes
//                 }
//                 else
//                 {
//                     message.ProcessData.pid = pid_childs;
//                     kill(pid_childs, SIGSTOP);
//                     //printf("process with %d recived at %d with running time %d \n  ", pid_childs,getClk(),message.ProcessData.runningtime);
//                     temp = (struct processData *)malloc(sizeof(struct processData));
//                     temp->arrivaltime = message.ProcessData.arrivaltime;
//                     temp->remainingTime = message.ProcessData.remainingTime;
//                     temp->waitingTime = message.ProcessData.waitingTime;
//                     temp->priority = message.ProcessData.priority;
//                     temp->pid = message.ProcessData.pid;
//                     temp->id = message.ProcessData.id;
//                     temp->runningtime = message.ProcessData.runningtime;
//                     printf("insersting %d \n", temp->pid);
//                     if (head == NULL)
//                     {
//                         push_(&head, *temp);
//                         pointer1 = head;
//                         printList(head);
//                     }
//                     else
//                     {
//                         insertAfter(pointer1, *temp);
//                         if (pointer1->next == NULL)
//                             printf("Next is NULL\n");
//                         printList(head);
//                     }
//                     continue;
//                 }
//             }
//             else
//             {

//                 if (head == NULL)
//                 {
//                     printf(" No processes scheduler is sleeping\n");
//                     sleep(1);
//                 }
//                 else
//                 {
//                     //printf("remaining time = %d\n", pointer1->data.remainingTime);
//                     if (pointer1->data.remainingTime > Qunata)
//                     {
//                         pointer1->data.remainingTime = pointer1->data.remainingTime - Qunata;
                            
//                         for (int i = 0; i < Qunata; i++)
//                         {
//                             printf("here more\n");
//                             kill(pointer1->data.pid, SIGCONT);
//                             sleep(1);
//                             kill(pointer1->data.pid, SIGSTOP);

//                             rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
//                             // checking of the message recived
//                             if (rcvTag != -1)
//                             {
//                                 ///// There are processes at the message queue, so inserting them in the priority queue
//                                 ////// DO Stuff to to insert them in the priority queue
//                                 pid_childs = fork();
//                                 if (pid_childs == -1)
//                                     printf("error in forking Processes");

//                                 else if (pid_childs == 0)
//                                 {
//                                     char remaining_char[7];

//                                     sprintf(remaining_char, "%d", message.ProcessData.runningtime);

//                                     char *argv[] = {"./process.out", remaining_char, 0};
//                                     execve(argv[0], &argv[0], NULL); //// forking the processes
//                                 }
//                                 else
//                                 {
//                                     message.ProcessData.pid = pid_childs;
//                                     kill(pid_childs, SIGSTOP);
//                                     //printf("process with %d recived at %d with running time %d \n  ", pid_childs,getClk(),message.ProcessData.runningtime);
//                                     temp = (struct processData *)malloc(sizeof(struct processData));
//                                     temp->arrivaltime = message.ProcessData.arrivaltime;
//                                     temp->remainingTime = message.ProcessData.remainingTime;
//                                     temp->waitingTime = message.ProcessData.waitingTime;
//                                     temp->priority = message.ProcessData.priority;
//                                     temp->pid = message.ProcessData.pid;
//                                     temp->id = message.ProcessData.id;
//                                     temp->runningtime = message.ProcessData.runningtime;
//                                     //printf("insersting %d \n", temp->pid);
//                                     insertAfter(pointer1, *temp);
//                                 }
//                             }
//                         }
//                     }
//                     else
//                     {
//                         for (int i = 0; i < /*Qunata -*/ pointer1->data.remainingTime; i++)
//                         {
//                             kill(pointer1->data.pid, SIGCONT);
//                             sleep(1);
//                             kill(pointer1->data.pid, SIGSTOP);

//                             rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
//                             // checking of the message recived
//                             if (rcvTag != -1)
//                             {
//                                 ///// There are processes at the message queue, so inserting them in the priority queue
//                                 //printf("process recived at %d \n  ", getClk());
//                                    ////// DO Stuff to to insert them in the priority queue
//                                 pid_childs = fork();
//                                 if (pid_childs == -1)
//                                     printf("error in forking Processes");

//                                 else if (pid_childs == 0)
//                                 {
//                                     char remaining_char[7];

//                                     sprintf(remaining_char, "%d", message.ProcessData.runningtime);

//                                     char *argv[] = {"./process.out", remaining_char, 0};
//                                     execve(argv[0], &argv[0], NULL); //// forking the processes
//                                 }
//                                 else
//                                 {
//                                     message.ProcessData.pid = pid_childs;
//                                     kill(pid_childs, SIGSTOP);
//                                     temp = (struct processData *)malloc(sizeof(struct processData));
//                                     temp->arrivaltime = message.ProcessData.arrivaltime;
//                                     temp->remainingTime = message.ProcessData.remainingTime;
//                                     temp->waitingTime = message.ProcessData.waitingTime;
//                                     temp->priority = message.ProcessData.priority;
//                                     temp->pid = message.ProcessData.pid;
//                                     temp->id = message.ProcessData.id;
//                                     temp->runningtime = message.ProcessData.runningtime;
//                                     insertAfter(pointer1, *temp);
//                                 }
//                             }
//                             if (i ==Qunata -pointer1->data.remainingTime)
//                             {
//                                 kill(pointer1->data.pid, SIGCONT);
//                                 wait(NULL);
//                                 pointer2 = pointer1->next;
//                                 flag = 0;
//                                 deleteNode(&head, pointer1->data);
//                                 pointer1 = pointer2;
//                                 if (pointer1 == NULL)
//                                     pointer1 = head;
//                             }
//                         }
//                     }
//                     if (flag)
//                     {
//                         pointer1 = pointer1->next;
//                         if (pointer1 == NULL)
//                         {
//                             pointer1 = head;
//                         }
//                     }
//                     flag = 1;
//                 }
//             }
//         }
// }

void main(int argc, char * argv[])
{
    signal(SIGCHLD, hpfHandler);
    signal(SIGINT,clearResources);
    
    initClk();
    msgqid1 = msgget(16499, 0644);

    logFilePtr = fopen("scheduler.log", "w");
    if (logFilePtr == NULL)
    {
        printf("Unable to create log file for the scheduler ... \n");
    }
    

    if (strcmp(argv[1], "-HPF") == 0){
        printf("HPF Scheduler Initialized\n");
        HPF();
    }
    else if (strcmp(argv[1], "-SRTN") == 0){
        printf("SRTN Scheduler Initialized\n");
        SRTN();
    }
    else if (strcmp(argv[1], "-RR") == 0){
        printf("RR Scheduler Initialized\n");
        // RR();
    }
    


    // scheduler logic
    
    
}
void hpfHandler(int signum){
    numberOfFinishedProcesses++;
    // printf("Process %d has finished at time %d\n", runningProcess.pid, getClk());


    fprintf(logFilePtr, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %0.2f\n",
    getClk(),runningProcess.pid, runningProcess.arrival, runningProcess.runtime, 0, WaitTime, (getClk() - runningProcess.arrival) /*TA*/, (getClk() - runningProcess.arrival)/ runningProcess.runtime /*WTA*/);
    available = true;
    signal(SIGCHLD, hpfHandler);
}
void clearResources(int signum){
    msgctl(msgqid1, IPC_RMID, (struct msqid_ds *) 0);
   // msgctl(msgqid2, IPC_RMID, (struct msqid_ds *) 0);
    printf(" Interruption detected ... ");
    exit(0);
}