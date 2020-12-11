#include <stdio.h> //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include<string.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
//don't mess with this variable//
int *sharedMemoryAddress; //
//===============================

int getClk()
{
    return *sharedMemoryAddress;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int sharedMemoryID = shmget(SHKEY, 4, 0444);
    while ((int)sharedMemoryID == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        sharedMemoryID = shmget(SHKEY, 4, 0444);
    }
    sharedMemoryAddress = (int *)shmat(sharedMemoryID, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(sharedMemoryAddress);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

///////////////
//// Struct that will be used inside the queue in the procces genrator to store data about the process
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int pid;
    int remainingTime;
    int waitingTime;
};
//////////////////////////////////////////
//// Struct that will be used inside the scheduler

//////////////////////////////////////
//*Queue From Geeks for Geeks

// A linked list (LL) node to store a queue entry
struct QNode
{
    struct processData key;
    struct QNode *next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue
{
    struct QNode *front, *rear;
};

// An utility function to create a new linked list node.
struct QNode *newNode(struct processData k)
{

    struct QNode *temp = (struct QNode *)malloc(sizeof(struct QNode));
    temp->key.id = k.id;
    temp->key.arrivaltime = k.arrivaltime;
    temp->key.runningtime = k.runningtime;
    temp->key.priority = k.priority;
    temp->next = NULL;

    return temp;
}

// A utility function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to a queue q
void enQueue(struct Queue *q, struct processData k)
{
    // Create a new LL node
    struct QNode *temp = newNode(k);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove a key from given queue q
void deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return ;

    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    return ;
}

////////////////////////////////////////////////
///////////////////////////////////////////////
void insert_priority_Sorted(struct Queue *q, struct QNode *value)
{
    // Empty list case //

    if (q->front == NULL)
    {
        enQueue(q, value->key);

        return ;
    }

    // Insert beginning case //
    if (q->front->key.priority >= value->key.priority)
    {
        value->next = q->front;
        q->front = value;
        return ;
    }

    /*	if (q->front->key.priority== value->key.priority) {
			value->next=q->front->next;
            q->front->next=value;
			return true;
		}*/
    // Single node case //
    if (q->front->next == NULL)
    {
        if (q->front->key.priority < value->key.priority)
        {
            enQueue(q, value->key);
            //free(value);
            return ;
        }
    }
    // General case //
    struct QNode *p = q->front;

    struct QNode *h = p->next;
    while (h->key.priority < value->key.priority)
    {
        if (h->next == NULL)
        {
            enQueue(q, value->key);
            return ;
        }
        p = h;
        h = h->next;
    }

    value->next = h;
    p->next = value;
    return ;
}
////////////////////////
///////////////////////
//////////////////////
/*
struct node_t 
{
    int priority;
    struct processData* data;
} ;
 
struct heap_t
{
   struct node_t *nodes;
    int len;
    int size;
} ;
 
void push (struct heap_t *h, int priority, struct processData*data) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (struct node_t *)realloc(h->nodes, h->size * sizeof (struct node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority= priority;
    h->nodes[i].data = data;
    h->len++;
}
 
struct processData*pop (struct heap_t *h) {
    int i, j, k;
    if (!h->len) {
        return NULL;
    }
    struct processData* data = h->nodes[1].data;
 
    h->nodes[1] = h->nodes[h->len];
 
    h->len--;
 
    i = 1;
    while (i!=h->len+1) {
        k = h->len+1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}
 */

//////////////////////////////
//// print queue
void printQueue(struct Queue *q)
{
    struct QNode*p=q->front;
    while (p != NULL)
    {
        printf("%d %d %d %d\n", p->key.id, p->key.arrivaltime, p->key.runningtime, p->key.priority);
        p = p->next;
    }
    p=NULL;
}

/////////////////////////////////////////////////
///////////////////////////////////////////////////
/*
struct node_t
{
    int priority;
    struct processData *data;
};

struct heap_t
{
    struct node_t *nodes;
    int len;
    int size;
};

void push(struct heap_t *h, int priority, struct processData *data)
{
    if (h->len + 1 >= h->size)
    {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (struct node_t *)realloc(h->nodes, h->size * sizeof(struct node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority)
    {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = data;
    h->len++;
}

struct processData *pop(struct heap_t *h)
{
    int i, j, k;
    if (!h->len)
    {
        return NULL;
    }
    struct processData *data = h->nodes[1].data;

    h->nodes[1] = h->nodes[h->len];

    h->len--;

    i = 1;
    while (i != h->len + 1)
    {
        k = h->len + 1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority)
        {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority)
        {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}*/
typedef struct
{
    int priority;
    struct processData *data;
} node_t;

typedef struct
{
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push(heap_t *h, int priority, struct processData *data)
{
    if (h->len + 1 >= h->size)
    {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof(node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority)
    {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = data;
    h->len++;
}

struct processData *pop(heap_t *h)
{
    int i, j, k;
    if (!h->len)
    {
        return NULL;
    }
        struct processData *data = h->nodes[1].data;

    h->nodes[1] = h->nodes[h->len];

    h->len--;

    i = 1;
    while (i != h->len + 1)
    {
        k = h->len + 1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority)
        {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority)
        {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}

///////////////////////////////////////////
///////////////////////////////////////////
/// messeage queue for sending the information form the process genrator
/// the schudler
struct Gen_to_Sch
{
    long mtype;
    struct processData ProcessData;
};









// A linked list node 
/*struct Node { 
	struct processData data; 
	struct Node* next; 
	struct Node* prev; 
}; 

/* Given a reference (pointer to pointer) to the head of a list 
and an int, inserts a new node on the front of the list. 
void push_(struct Node** head_ref,  struct processData new_data) 
{ 
	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime; 

	new_node->next = (*head_ref); 
	new_node->prev = NULL; 

	if ((*head_ref) != NULL) 
		(*head_ref)->prev = new_node; 

	(*head_ref) = new_node; 
} 

/* Given a node as next_node, insert a new node before the given node 
void insertBefore(struct Node** head_ref, struct Node* next_node, struct processData new_data) 
{ 
	/*1. check if the given next_node is NULL 
	if (next_node == NULL) { 
		printf("the given next node cannot be NULL"); 
		return; 
	} 

	/* 2. allocate new node 
	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	/* 3. put in the data 
	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;

	/* 4. Make prev of new node as prev of next_node 
	new_node->prev = next_node->prev; 

	/* 5. Make the prev of next_node as new_node 
	next_node->prev = new_node; 

	/* 6. Make next_node as next of new_node 
	new_node->next = next_node; 

	/* 7. Change next of new_node's previous node 
	if (new_node->prev != NULL) 
		new_node->prev->next = new_node; 
	/* 8. If the prev of new_node is NULL, it will be 
	the new head node 
	else
		(*head_ref) = new_node; 
	
} 
/* Given a node as prev_node, insert a new node after the given node 
void insertAfter(struct Node* prev_node, struct processData new_data) 
{ 
	/*1. check if the given prev_node is NULL 
	if (prev_node == NULL) { 
		printf("the given previous node cannot be NULL\n"); 
		return; 
	} 

	/* 2. allocate new node 
	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	/* 3. put in the data 
	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;

	/* 4. Make next of new node as next of prev_node 
	new_node->next = prev_node->next; 

	/* 5. Make the next of prev_node as new_node 
	prev_node->next = new_node; 

	/* 6. Make prev_node as previous of new_node 
	new_node->prev = prev_node; 

	/* 7. Change previous of new_node's next node 
	if (new_node->next != NULL) 
		new_node->next->prev = new_node; 
} 

/* Given a reference (pointer to pointer) to the head 
of a DLL and an int, appends a new node at the end 
void append(struct Node** head_ref, struct processData new_data) 
{ 
	/* 1. allocate node 
	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	struct Node* last = *head_ref; /* used in step 5

	/* 2. put in the data 
	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;

	/* 3. This new node is going to be the last node, so 
		make next of it as NULL
	new_node->next = NULL; 

	/* 4. If the Linked List is empty, then make the new 
		node as head 
	if (*head_ref == NULL) { 
		new_node->prev = NULL; 
		*head_ref = new_node; 
		return; 
	} 

	/* 5. Else traverse till the last node 
	while (last->next != NULL) 
		last = last->next; 

	/* 6. Change the next of last node 
	last->next = new_node; 

	/* 7. Make last node as previous of new node 
	new_node->prev = last; 

	return; 
} 


// This function prints contents of linked list starting from the given node 
void printList(struct Node* node) 
{ 
	struct Node* last; 
	printf("\nTraversal in forward direction \n"); 
	while (node != NULL) { 
		printf(" %d ", node->data.id); 
		last = node; 
		node = node->next; 
	} 

	printf("\nTraversal in reverse direction \n"); 
	while (last != NULL) { 
		printf(" %d ", last->data.id); 
		last = last->prev; 
	} 
} */
/*void deleteNode(struct Node** head_ref, struct Node* del) 
{ 
    /* base case 
    if (*head_ref == NULL || del == NULL) 
        return; 
  
    /* If node to be deleted is head node 
    if (*head_ref == del) 
        *head_ref = del->next; 
  
    /* Change next only if node to be deleted is NOT the last node 
    if (del->next != NULL) 
        del->next->prev = del->prev; 
  
    /* Change prev only if node to be deleted is NOT the first node 
    if (del->prev != NULL) 
        del->prev->next = del->next; 
  
    /* Finally, free the memory occupied by del
    free(del); 
    return; 
} 
  */
struct Node { 
    struct processData  data; 
    struct Node* next; 
}; 
void printList(struct Node* n) 
{ 
    while (n != NULL) { 
        printf(" %d ", n->data.id); 
        n = n->next; 
    } 
} 
void push_(struct Node** head_ref, struct processData  new_data) 
{ 
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
   
    /* 2. put in the data  */
   new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;
   
    /* 3. Make next of new node as head */
    new_node->next = (*head_ref); 
   
    /* 4. move the head to point to the new node */
    (*head_ref)    = new_node; 
    printf("DONE\n");
} 
/* Given a node prev_node, insert a new node after the given 
prev_node */
void insertAfter(struct Node* prev_node, struct processData new_data) 
{ 
	/*1. check if the given prev_node is NULL */
	if (prev_node == NULL) 
	{ 
	printf("the given previous node cannot be NULL\n");	 
	return; 
	} 
		
	/* 2. allocate new node */
	struct Node* new_node =(struct Node*) malloc(sizeof(struct Node)); 

	/* 3. put in the data */
	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;

	/* 4. Make next of new node as next of prev_node */
	new_node->next = prev_node->next; 

	/* 5. move the next of prev_node as new_node */
	prev_node->next = new_node; 
} 
/* Given a reference (pointer to pointer) to the head 
of a list and an int, appends a new node at the end */
void append(struct Node** head_ref, struct processData new_data) 
{ 
	/* 1. allocate node */
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 

	struct Node *last = *head_ref; /* used in step 5*/

	/* 2. put in the data */
	new_node->data.arrivaltime = new_data.arrivaltime;
	new_node->data.id = new_data.id;
	new_node->data.runningtime = new_data.runningtime;
	new_node->data.remainingTime = new_data.remainingTime;
	new_node->data.priority = new_data.priority;
	new_node->data.pid = new_data.pid; 
	new_node->data.waitingTime= new_data.waitingTime;

	/* 3. This new node is going to be the last node, so make next 
		of it as NULL*/
	new_node->next = NULL; 

	/* 4. If the Linked List is empty, then make the new node as head */
	if (*head_ref == NULL) 
	{ 
	*head_ref = new_node; 
	return; 
	} 
	
	/* 5. Else traverse till the last node */
	while (last->next != NULL) 
		last = last->next; 

	/* 6. Change the next of last node */
	last->next = new_node; 
	return;	 
} 
/* Given a reference (pointer to pointer) to the head of a list 
   and a key, deletes the first occurrence of key in linked list */
void deleteNode(struct Node **head_ref, struct processData key) 
{ 
    // Store head node 
    struct Node* temp = *head_ref, *prev; 
  
    // If head node itself holds the key to be deleted 
    if (temp != NULL && temp->data.pid == key.pid) 
    { 
        *head_ref = temp->next;   // Changed head 
        free(temp);               // free old head 
        return; 
    } 
  
    // Search for the key to be deleted, keep track of the 
    // previous node as we need to change 'prev->next' 
    while (temp != NULL && temp->data.pid != key.pid) 
    { 
        prev = temp; 
        temp = temp->next; 
    } 
  
    // If key was not present in linked list 
    if (temp == NULL) return; 
  
    // Unlink the node from linked list 
    prev->next = temp->next; 
  
    free(temp);  // Free memory 
} 