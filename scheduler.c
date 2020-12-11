#include "headers.h"

#pragma region Glabal varaibles

heap_t *proccessTable;
struct Node *head;
struct Node *pointer1;
struct Node *pointer2;
int flag;
FILE *fp; // output file variable
char dummy[2000];
char out[100000];
int pid_of_currnet_process;

#pragma endregion

#pragma region Signal handlers

//SIGNINT handler
void cleanup(int signum)
{
    printf("Schedule terminating!\n");
    printf("%s\n", out);
    fclose(fp);
    destroyClk(true);
    exit(0);
}

void child_handler(int signum)
{
    if (proccessTable->nodes[1].data->remainingTime == proccessTable->nodes[1].data->runningtime)
        return;

    if (proccessTable->nodes[1].data->remainingTime <= 0)
    {
        printf("popping %d\n", proccessTable->nodes[1].data->pid);
        wait(NULL);
        sprintf(dummy, "At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk(), proccessTable->nodes[1].data->id, proccessTable->nodes[1].data->arrivaltime, proccessTable->nodes[1].data->runningtime, 0, getClk() - (proccessTable->nodes[1].data->arrivaltime) - (proccessTable->nodes[1].data->runningtime));
        strcat(out, dummy);
        //Finished*/
        pop(proccessTable);
        //pid_of_currnet_process = 0;
    }
    else
    {

        sprintf(dummy, "HI time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), proccessTable->nodes[1].data->id, proccessTable->nodes[1].data->arrivaltime, proccessTable->nodes[1].data->runningtime, 0, getClk() - (proccessTable->nodes[1].data->arrivaltime) - (proccessTable->nodes[1].data->runningtime));
        strcat(out, dummy);
        //stoped
    }
}

void child_handler_Round_Robin(int signum)
{
    /* if (pointer1->data->remainingTime <= 0)
    {
        printf("popping %d\n", pointer1->data->pid);
        wait(NULL);
        pointer2 = pointer1->next;
        if (pointer2 == NULL)
            pointer2 = head;
        deleteNode(&head, pointer1);
        pointer1 = pointer2;
    }
    else
    {
        printf("stoped %d\n", pointer1->data->pid);
    }*/
}

#pragma endregion

#pragma region Main function
int main(int argc, char *argv[])
{
    FILE *fp;
    int myInt = 5;
    fp = fopen("Output.txt", "w"); // "w" means that we are going to write on this file
                                   //Don't forget to close the file when finished
    signal(SIGINT, cleanup);

    int pid_childs;
    int pid_finished;
    printf("Schudler iniatiated\n");
    initClk();
    #pragma region Preemptive Highest Priority First (HPF)

    if (*argv[1] == 'a' || *argv[1] == 'A') // Non pre-emptive priority
    {

        printf("Non preemptive HPF is in action\n");
        proccessTable = (heap_t *)calloc(1, sizeof(heap_t));

        /////////////////////////////////////////////////////////////////
        //////// receive the messeage
        struct Gen_to_Sch message;
        int msgid;
        msgid = msgget(1, 0666);
        char remaining_char[7];
        // msgrcv to receive message
        struct processData *temp;
        while (1)
        {
            int rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
            // checking of the message recived
            if (rcvTag == -1)
            {
                ///// No proceeses at the message queue
                //printf("nothing at %d\n\n", getClk());
                if (!proccessTable->len)
                {
                    printf(" No scheduler is sleeping\n");

                    sleep(1);
                }
                else
                {
                    printf("%d\n", proccessTable->nodes[1].data->priority);
                    if (kill(proccessTable->nodes[1].data->pid, SIGCONT) == 0)
                    {
                        int status;
                        pid_finished = wait(&status);
                        printf("process Finished\n");
                        pop(proccessTable);
                    }
                    else
                    {
                        printf(" process in action scheduler is sleeping\n");
                        sleep(1);
                    }
                }
            }
            else
            {
                //// There are processes at the message queue, so inserting them in the priority queue
                //printf("process recived at %d \n  ", getClk());
                ////// DO Stuff to to insert them in the priority queue
                pid_childs = fork();
                if (pid_childs == -1)
                    printf("error in forking Processes");

                else if (pid_childs == 0)
                {
                    char remaining_char[7];

                    sprintf(remaining_char, "%d", message.ProcessData.runningtime);

                    char *argv[] = {"./process.out", remaining_char, 0};
                    execve(argv[0], &argv[0], NULL); //// forking the processes
                }
                else
                {
                    message.ProcessData.pid = pid_childs;
                    kill(pid_childs, SIGSTOP);
                    temp = (struct processData *)malloc(sizeof(struct processData));
                    temp->arrivaltime = message.ProcessData.arrivaltime;
                    temp->remainingTime = message.ProcessData.remainingTime;
                    temp->waitingTime = message.ProcessData.waitingTime;
                    temp->priority = message.ProcessData.priority;
                    temp->pid = message.ProcessData.pid;
                    temp->id = message.ProcessData.id;
                    temp->runningtime = message.ProcessData.runningtime;
                    push(proccessTable, message.ProcessData.priority, temp);
                    printf("highest Priority is %d\n", proccessTable->nodes[1].priority);
                    continue;
                }
            }
        }
    }

    #pragma endregion

    #pragma region Shortest Remaining Time Next (SRTN)

    else if (*argv[1] == 'b' || *argv[1] == 'B')
    {

        printf("Shortest Remaining Time Next is in action \n");
        proccessTable = (heap_t *)calloc(1, sizeof(heap_t));

        /////////////////////////////////////////////////////////////////
        //////// receive the messeage
        struct Gen_to_Sch message;
        int msgid;
        msgid = msgget(1, 0666);
        char remaining_char[7];
        // msgrcv to receive message
        struct processData *temp;
        /////
        pid_of_currnet_process = 0;
        while (1)
        {
            int rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
            // checking of the message recived
            if (rcvTag == -1)
            {
                ///// No proceeses at the message queue
                //printf("nothing at %d\n\n", getClk());
                if (!proccessTable->len)
                {
                    printf(" No scheduler is sleeping\n");

                    sleep(1);
                }
                else
                {

                    signal(SIGCHLD, child_handler);

                    if (proccessTable->nodes[1].data->pid == pid_of_currnet_process)
                    {
                        printf("remaining %d\n", proccessTable->nodes[1].data->pid);
                        sleep(1);
                        proccessTable->nodes[1].data->remainingTime = proccessTable->nodes[1].data->remainingTime - 1;
                        proccessTable->nodes[1].priority = proccessTable->nodes[1].priority - 1;
                    }
                    else if (pid_of_currnet_process == 0)
                    {
                        pid_of_currnet_process = proccessTable->nodes[1].data->pid;
                        sprintf(dummy, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), proccessTable->nodes[1].data->id, proccessTable->nodes[1].data->arrivaltime, proccessTable->nodes[1].data->runningtime, 0, getClk() - (proccessTable->nodes[1].data->arrivaltime) - (proccessTable->nodes[1].data->runningtime));
                        strcat(out, dummy);
                        kill(pid_of_currnet_process, SIGCONT);
                        printf("started %d\n", proccessTable->nodes[1].data->pid);
                        sleep(1);
                        proccessTable->nodes[1].data->remainingTime = proccessTable->nodes[1].data->remainingTime - 1;
                        proccessTable->nodes[1].priority = proccessTable->nodes[1].priority - 1;
                    }
                    else
                    {

                        kill(pid_of_currnet_process, SIGSTOP);
                        signal(SIGCHLD, child_handler);
                        pid_of_currnet_process = proccessTable->nodes[1].data->pid;
                        kill(pid_of_currnet_process, SIGCONT);
                        sprintf(dummy, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), proccessTable->nodes[1].data->id, proccessTable->nodes[1].data->arrivaltime, proccessTable->nodes[1].data->runningtime, 0, getClk() - (proccessTable->nodes[1].data->arrivaltime) - (proccessTable->nodes[1].data->runningtime));
                        strcat(out, dummy);
                        sleep(1);
                        proccessTable->nodes[1].data->remainingTime = proccessTable->nodes[1].data->remainingTime - 1;
                        proccessTable->nodes[1].priority = proccessTable->nodes[1].priority - 1;
                    }
                }
            }
            else
            {
                //// There are processes at the message queue, so inserting them in the priority queue
                //printf("process recived at %d \n  ", getClk());
                ////// DO Stuff to to insert them in the priority queue
                pid_childs = fork();
                if (pid_childs == -1)
                    printf("error in forking Processes");

                else if (pid_childs == 0)
                {
                    char remaining_char[7];

                    sprintf(remaining_char, "%d", message.ProcessData.runningtime);

                    char *argv[] = {"./process.out", remaining_char, 0};
                    execve(argv[0], &argv[0], NULL); //// forking the processes
                }
                else
                {
                    message.ProcessData.pid = pid_childs;
                    signal(SIGCHLD, SIG_IGN);
                    kill(pid_childs, SIGSTOP);
                      signal(SIGCHLD, child_handler);

                    //printf("process with %d recived at %d with running time %d \n  ", pid_childs,getClk(),message.ProcessData.runningtime);
                    temp = (struct processData *)malloc(sizeof(struct processData));
                    temp->arrivaltime = message.ProcessData.arrivaltime;
                    temp->remainingTime = message.ProcessData.remainingTime;
                    temp->waitingTime = message.ProcessData.waitingTime;
                    temp->priority = message.ProcessData.priority;
                    temp->pid = message.ProcessData.pid;
                    temp->id = message.ProcessData.id;
                    temp->runningtime = message.ProcessData.runningtime;
                    push(proccessTable, message.ProcessData.remainingTime, temp);
                    printf("%d highest Priority is %d\n", proccessTable->nodes[1].data->pid, proccessTable->nodes[1].data->priority);
                    continue;
                }
            }
        }
    }
    
    #pragma endregion

    #pragma region Round Robin (RR)

    else if (*argv[1] == 'c' || *argv[1] == 'C' )
    {
        printf("Round Robin in action\n");
        /////////////////////////////////////////////////////////////////
        //////// receive the messeage
        //signal(SIGCHLD, child_handler_Round_Robin);
        struct Gen_to_Sch message;
        int msgid;
        msgid = msgget(1, 0666);
        char remaining_char[7];
        int Qunata = atoi(argv[2]);
        printf("Quanta %d\n", Qunata);
        // msgrcv to receive message
        struct processData *temp;
        head = NULL;
        flag = 1;
        pointer1 = head;
        while (1)
        {
            int rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
            // checking of the message recived
            if (rcvTag != -1)
            {
                ///// There are processes at the message queue, so inserting them in the priority queue
                ////// DO Stuff to to insert them in the priority queue
                pid_childs = fork();
                if (pid_childs == -1)
                    printf("error in forking Processes");

                else if (pid_childs == 0)
                {
                    char remaining_char[7];

                    sprintf(remaining_char, "%d", message.ProcessData.runningtime);

                    char *argv[] = {"./process.out", remaining_char, 0};
                    execve(argv[0], &argv[0], NULL); //// forking the processes
                }
                else
                {
                    message.ProcessData.pid = pid_childs;
                    kill(pid_childs, SIGSTOP);
                    //printf("process with %d recived at %d with running time %d \n  ", pid_childs,getClk(),message.ProcessData.runningtime);
                    temp = (struct processData *)malloc(sizeof(struct processData));
                    temp->arrivaltime = message.ProcessData.arrivaltime;
                    temp->remainingTime = message.ProcessData.remainingTime;
                    temp->waitingTime = message.ProcessData.waitingTime;
                    temp->priority = message.ProcessData.priority;
                    temp->pid = message.ProcessData.pid;
                    temp->id = message.ProcessData.id;
                    temp->runningtime = message.ProcessData.runningtime;
                    printf("insersting %d \n", temp->pid);
                    if (head == NULL)
                    {
                        push_(&head, *temp);
                        pointer1 = head;
                        printList(head);
                    }
                    else
                    {
                        insertAfter(pointer1, *temp);
                        if (pointer1->next == NULL)
                            printf("Next is NULL\n");
                        printList(head);
                    }
                    continue;
                }
            }
            else
            {

                if (head == NULL)
                {
                    printf(" No processes scheduler is sleeping\n");
                    sleep(1);
                }
                else
                {
                    //printf("remaining time = %d\n", pointer1->data.remainingTime);
                    if (pointer1->data.remainingTime > Qunata)
                    {
                        pointer1->data.remainingTime = pointer1->data.remainingTime - Qunata;
                            
                        for (int i = 0; i < Qunata; i++)
                        {
                            printf("here more\n");
                            kill(pointer1->data.pid, SIGCONT);
                            sleep(1);
                            kill(pointer1->data.pid, SIGSTOP);

                            rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
                            // checking of the message recived
                            if (rcvTag != -1)
                            {
                                ///// There are processes at the message queue, so inserting them in the priority queue
                                ////// DO Stuff to to insert them in the priority queue
                                pid_childs = fork();
                                if (pid_childs == -1)
                                    printf("error in forking Processes");

                                else if (pid_childs == 0)
                                {
                                    char remaining_char[7];

                                    sprintf(remaining_char, "%d", message.ProcessData.runningtime);

                                    char *argv[] = {"./process.out", remaining_char, 0};
                                    execve(argv[0], &argv[0], NULL); //// forking the processes
                                }
                                else
                                {
                                    message.ProcessData.pid = pid_childs;
                                    kill(pid_childs, SIGSTOP);
                                    //printf("process with %d recived at %d with running time %d \n  ", pid_childs,getClk(),message.ProcessData.runningtime);
                                    temp = (struct processData *)malloc(sizeof(struct processData));
                                    temp->arrivaltime = message.ProcessData.arrivaltime;
                                    temp->remainingTime = message.ProcessData.remainingTime;
                                    temp->waitingTime = message.ProcessData.waitingTime;
                                    temp->priority = message.ProcessData.priority;
                                    temp->pid = message.ProcessData.pid;
                                    temp->id = message.ProcessData.id;
                                    temp->runningtime = message.ProcessData.runningtime;
                                    //printf("insersting %d \n", temp->pid);
                                    insertAfter(pointer1, *temp);
                                }
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < /*Qunata -*/ pointer1->data.remainingTime; i++)
                        {
                            kill(pointer1->data.pid, SIGCONT);
                            sleep(1);
                            kill(pointer1->data.pid, SIGSTOP);

                            rcvTag = msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT);
                            // checking of the message recived
                            if (rcvTag != -1)
                            {
                                ///// There are processes at the message queue, so inserting them in the priority queue
                                //printf("process recived at %d \n  ", getClk());
                                   ////// DO Stuff to to insert them in the priority queue
                                pid_childs = fork();
                                if (pid_childs == -1)
                                    printf("error in forking Processes");

                                else if (pid_childs == 0)
                                {
                                    char remaining_char[7];

                                    sprintf(remaining_char, "%d", message.ProcessData.runningtime);

                                    char *argv[] = {"./process.out", remaining_char, 0};
                                    execve(argv[0], &argv[0], NULL); //// forking the processes
                                }
                                else
                                {
                                    message.ProcessData.pid = pid_childs;
                                    kill(pid_childs, SIGSTOP);
                                    temp = (struct processData *)malloc(sizeof(struct processData));
                                    temp->arrivaltime = message.ProcessData.arrivaltime;
                                    temp->remainingTime = message.ProcessData.remainingTime;
                                    temp->waitingTime = message.ProcessData.waitingTime;
                                    temp->priority = message.ProcessData.priority;
                                    temp->pid = message.ProcessData.pid;
                                    temp->id = message.ProcessData.id;
                                    temp->runningtime = message.ProcessData.runningtime;
                                    insertAfter(pointer1, *temp);
                                }
                            }
                            if (i ==Qunata -pointer1->data.remainingTime)
                            {
                                kill(pointer1->data.pid, SIGCONT);
                                wait(NULL);
                                pointer2 = pointer1->next;
                                flag = 0;
                                deleteNode(&head, pointer1->data);
                                pointer1 = pointer2;
                                if (pointer1 == NULL)
                                    pointer1 = head;
                            }
                        }
                    }
                    if (flag)
                    {
                        pointer1 = pointer1->next;
                        if (pointer1 == NULL)
                        {
                            pointer1 = head;
                        }
                    }
                    flag = 1;
                }
            }
        }
    }

    #pragma endregion
    
    #pragma region Shortest Job First (SJF).

    else{

    }

    #pragma endregion

    // destroyClk(true);
}
#pragma endregion
