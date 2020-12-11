#include <stdio.h>
#include <stdbool.h>

#define MAX 20

struct Data
{
	int id;
	int arrival;
	int runtime;
	int remainingT;
	int priority;
	int last_processed;
	char status[10];
};

struct Queue
{
	
	struct Data dataArray[MAX];
	int front;
	int rear;
	// int itemCount;
	
};

void initializeQueue(struct Queue * queue)
{
	queue->front = 0;
	queue->rear = 0;
	// queue.itemCount = 0;
}

void resetQueue(struct Queue * queue)
{
	queue->front = 0;
	queue->rear = 0;
	// queue.itemCount = 0;
}

struct Data peek(struct Queue queue) {
	return queue.dataArray[queue.front];
}

bool isEmpty(struct Queue queue) {
   return queue.front == queue.rear;
}

bool isFull(struct Queue queue) {
   return queue.rear == MAX;
}

int size(struct Queue queue) {
   return queue.rear;
}  

void insert(struct Queue queue, struct Data data) {

   if(!isFull(queue)) {    
	  queue.dataArray[queue.rear].id = data.id;
	  queue.dataArray[queue.rear].arrival = data.arrival;
	  queue.dataArray[queue.rear].runtime = data.runtime;
	  // printf("Assigned item ID: %d, Arrival: %d\n", queue.dataArray->id, queue.dataArray->arrival);
	  queue.rear++;
	  // queue.itemCount++;
   }
   else
   {
	   printf("Queue is Full!");
   }
}

struct Data removeData(struct Queue queue) {
   struct Data data;
   // queue.front++;
   data.id = queue.dataArray[queue.front].id;
   data.arrival = queue.dataArray[queue.front].arrival;	
   data.runtime = queue.dataArray[queue.front].runtime;
	
	for (int i = 0; i < queue.rear-1; i++)
	{
		queue.dataArray[i] = queue.dataArray[i+1];
	}
	
   queue.rear--;
   return data;  
}
