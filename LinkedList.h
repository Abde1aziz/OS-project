// this code is compined from rosseta code

typedef struct
{
	int id;
	int arrival;
	int priority;
	bool isRunning;
	int runtime;
} pData;


typedef struct elem{
	pData data;
	struct elem* next;
}cell;
typedef cell* list;

list addToList(list a,pData daTa){

 list iter, temp;
 int i=0;

 if(a==NULL){
   a = (list)malloc(sizeof(cell));
   a->data = daTa;
   a->next = NULL;
 }
 else{
   iter = a;

   while(iter->next!=NULL){
     iter = iter->next;
   }

   temp = (list)malloc(sizeof(cell));
   temp->data = daTa;
   temp->next = NULL;

   iter->next = temp;
 }
 return a;
}
//this adds a node to the list sorted bt time

list insertSorted(list a,pData daTa){
	list iter, temp;
  int i=0;

  if(a==NULL){
    a = (list)malloc(sizeof(cell));
    a->data = daTa;
    a->next = NULL;
  }
  else{
    iter = a;
		//nextIter = iter->next;

		//check if to insert in the beginning
		if(daTa.remainingT < iter->data.remainingT)
		{
			temp = (list)malloc(sizeof(cell));
			temp->data = daTa;
			temp->next = iter;

			a = temp;
		}
		//insert anywhere else
    while(iter->next!=NULL){

			if(daTa.remainingT < iter->next->data.remainingT)
			{
				temp = (list)malloc(sizeof(cell));
				temp->data = daTa;
				temp->next = iter->next;
				iter->next = temp;
				return a;

			}
			iter = iter->next;
    }
		//insert in the end
    temp = (list)malloc(sizeof(cell));
    temp->data = daTa;
    temp->next = NULL;

    iter->next = temp;
  }
  return a;
}

//check if you should pass a data structure
list DeqFromList(list a,pData *d){

 list iter;

 if(a!=NULL){
   iter = a;
     a = a->next;
     iter->next = NULL;
     //free(iter);
		 *d = iter->data;

 }
 return a;
}

void printList(list a){
 list temp = a;

 printf("List contains following elements : \n");

 while(temp!=NULL){
  // printf("%d ",temp->data->id);
   temp = temp->next;
 }
}
// this comment is from khairy
//may be this main is and example to show how it works
/*int main(int argC,char* argV[])
{
 list a = NULL;
 int i;

 if(argC == 1)
   printf("Usage : %s <list of integers to be inserted into the list>",argV[0]);
 else{
   for(i=2;i<=argC;i++)
     a = addToList(a,atoi(argV[i-1]));

   printList(a);

   do{
     printf("\nEnter position of element to be removed (1-%d) : ",argC-1);

     scanf("%d",&i);

     if(i>0 && i<=argC-1){
       a = deleteFromList(a,i);
       printList(a);
     }
   }while(i>argC-1||i<=0);
 }
 return 0;
}
*/
/*
//traversal
struct link *first;
// ...
struct link *iter;
for(iter = first; iter != NULL; iter = iter->next) {
  // access data, e.g. with iter->data
}
*/
