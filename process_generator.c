#include "headers.h"
#include "queue.h"
#define _GNU_SOURCE

pid_t pid;
key_t msgqid1;



int processes_count;
// int** processes_info;

void clearResources(int signum);

// count lines in a file 
int count_lines(FILE *fp)
{
    int lines = 1;
    char chr;
    
    chr = getc(fp);
    while (chr != EOF)
    {
        if (chr == '\n')
        {
            lines++;
        }
        
        chr = getc(fp);
    }
    
    fclose(fp);
    return lines;
}

int** create_array(int m, int n)
{
    int* values = calloc(m*n, sizeof(float));
    int** rows = malloc(n*sizeof(int*));
    for (int i = 0; i < n; i++)
    {
        rows[i] = values + i*m;
    }
    return rows;
}

int** read_processes()
{
    // implement read file functionality here
    FILE *fp1;
    FILE *fp2;
    ssize_t read;
    size_t len;
    char * line = NULL;
    // int line_count;
    int c = 0;
    int temp_id;
    int temp_arrival;
    int temp_runtime;
    int temp_priority;
    
    fp1 = fopen("processes.txt", "r");
    fp2 = fopen("processes.txt", "r");
    
    if (fp1 == NULL || fp2 == NULL)
    {
        printf("No such file ... \n");
        exit(EXIT_FAILURE);
    }
    
    processes_count = count_lines(fp1);
    // printf("Line Count: %d\n", line_count);
    int ids[processes_count];
    int arrivals[processes_count];
    int runtimes[processes_count];
    int priorities[processes_count];
    
    int** ret_arr = create_array(4, processes_count-1);
    
    
    while ((read = getline(&line, &len, fp2)) != -1)
    {
        // printf("%s\n", line);
        if (c == 0)
        {
            c++;
        }
        else
        {
            c++;
            sscanf(line, "%d\t%d\t%d\t%d", &temp_id, &temp_arrival, &temp_runtime, &temp_priority);   
        }
        
        // passing parameters to list stats
        ids[c-2] = temp_id;
        arrivals[c-2] = temp_arrival;
        runtimes[c-2] = temp_runtime;
        priorities[c-2] = temp_priority;
    }
    
    // passing local arrays to return array 
    // ret_arr = {ids, arrivals, runtimes, priorities};
    for (int i = 0; i < processes_count-1; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (j == 0)
            {
                ret_arr[i][j] = ids[i];
            }   
            else if (j == 1)
            {
                ret_arr[i][j] = arrivals[i];
            }   
            else if (j == 2)
            {
                ret_arr[i][j] = runtimes[i];
            }
            else
            {
                ret_arr[i][j] = priorities[i];
            }  
            // printf("Data: %d  ",ret_arr[i][j]);  
        }
        // printf("\n");
    }
    
    // closing file and  disposing pointers
    if (line)
    {
        free(line);
    }
    // exit(EXIT_SUCCESS); /*can cause the function to return NULL, be careful */
    
    return ret_arr;
}

// function to send process 
void send_process(int id, struct Queue prc_queue, key_t msgqid)
{
	int send_val;
	
	struct Data sentData = prc_queue.dataArray[id];
	
	send_val = msgsnd(msgqid1, &sentData, sizeof(sentData), !IPC_NOWAIT);

	if (send_val == -1)
	{
		perror("Error in send ");
	}
	else
	{
		printf("Process Data Sent Successfully\n");
	}
}

// function to read input processes and store them into a queue (working)
struct Queue readProcessesInfo(struct Queue prc_queue, int** processes_info)
{
	struct Data processes_data;
	printf("Beginning of ReadAndSendFunction ... \n");

	
	// read processes from Matrix
	for (int i = 0; i < processes_count-2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (j == 0)
			{
				processes_data.id = processes_info[i][j];
			}
			else if (j == 1)
			{
				processes_data.arrival = processes_info[i][j];
			}
			else if (j == 2)
			{
				processes_data.runtime = processes_info[i][j];
			}
            else if(j == 3)
            {
                processes_data.priority = processes_info[i][j];
            }
            
			// insert(processes_queue, processes_data);  // not working
		}
		prc_queue.dataArray[prc_queue.rear] = processes_data;
		prc_queue.rear++;
		// printf("Inserted: %d\n", processes_data.runtime);
	}
	printf("Size of sent buffer: %d\n", size(prc_queue));
	// (working) printf("Id: %d\tRuntime: %d\tArrival: %d\n", prc_queue.dataArray[prc_queue.front].id, prc_queue.dataArray[prc_queue.front].runtime, prc_queue.dataArray[prc_queue.front].arrival);
	return prc_queue;
}

void initiate_scheduler(char choice[])
{

    char *s_argv[] = {"./scheduler.out", choice, 0};
	execve(s_argv[0], &s_argv[0], NULL);
}

void initiate_RR_scheduler()
{

    char *s_argv[] = {"./scheduler.out", "-RR", 0};
	execve(s_argv[0], &s_argv[0], NULL);
}

void initiate_SRTN_scheduler()
{

    char *s_argv[] = {"./scheduler.out", "-SRTN", 0};
	execve(s_argv[0], &s_argv[0], NULL);
}
void initiate_clock()
{
    char *c_argv[] = {"./clk.out", 0};
	execve(c_argv[0], &c_argv[0], NULL);
}

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    FILE *fp;
    ssize_t read;
    size_t len;
    pid_t ss_pid;
    pid_t sc_pid;
    char * line = NULL;
    char ch[1];
    char choice[8];
    int** processes_info;
    
   //reading from file 
    pid = getpid();
	msgqid1 = msgget(16499, IPC_CREAT | 0644);
    int line_count = 0;
    fp = fopen("processes.txt", "r");
    if (fp != NULL)
    {
        processes_count = count_lines(fp);
        printf("line count: %d\n", processes_count);
        processes_info = read_processes();
        for (int i = 0; i < processes_count-1; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                printf("%d  ", processes_info[i][j]);
            }
            printf("\n");
        }
    }
    else
    {
        printf("No such file ... \n");
    }

    //choose the algorithm
    printf("Choose scheduler from the list below: \n1. Non-preemptive Highest Priority First (HPF) \n2. Shortest Remaining time next (SRTN) \n3. Round Robin (RR) \nyour choice: ");
    scanf("%s", (char *)ch);
    if (strcmp(ch, "1") == 0)
    {
        strcpy(choice,"-HPF");
        printf("HPF is selected \n");

    }
    else if (strcmp(ch, "2") == 0)
    {
        strcpy(choice,"-SRTN");
        printf("SRTN is selected \n");

    }
    else if (strcmp(ch, "3") == 0)
    {
        strcpy(choice,"-RR");
        printf("RR is selected \n");
    }
    // Scheduler
    int sch_fork = fork();
    if(sch_fork == -1)
    {
        perror("Error in initializing scheduler ..\n");
    }
    else if(sch_fork == 0)
    {
        sleep(3);
        //intialtizing the scheduler with the right algorithm
        initiate_scheduler(choice);
    }
    else{

        //forking the clock
        int clock_fork = fork();
        if(clock_fork == -1)
        {
            perror("Error in initializing clock ..\n");
        }
        else if(clock_fork == 0){
            initiate_clock();
        }else{
            sleep(1);
            initClk();

        //creating the message queue to send the data
            struct Queue prc_queue;
			initializeQueue(&prc_queue);
			prc_queue = readProcessesInfo(prc_queue, processes_info);
			int c = 0;
			while (getClk() <= prc_queue.dataArray[prc_queue.rear-1].arrival)
			{
				if (getClk() == prc_queue.dataArray[c].arrival)
				{
					send_process(c, prc_queue, msgqid1);
					c++;
					sleep(1);
				}
			}

            int sc_status;
			sc_pid = wait(&sc_status);
			if(!(sc_status & 0x00FF))
				printf("\n CLOCK with pid %d terminated with exit code %d\n", sc_pid, sc_status>>8);
        }
        int ss_status;
        ss_pid = wait(&ss_status);
        if(!(ss_status & 0x00FF))
        	printf("\n Scheduler with pid %d terminated with exit code %d\n", ss_pid, ss_status>>8);
    } 
	
    destroyClk(true);
    return 0;
}
void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    // handle kill signal
    msgctl(msgqid1, IPC_RMID, (struct msqid_ds *) 0);
   // msgctl(msgqid2, IPC_RMID, (struct msqid_ds *) 0);
    printf(" Interruption detected ... ");
    exit(0);
}
