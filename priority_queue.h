

typedef struct
{
	int pid;
	int arrival;
	int priority;
	bool isRunning;
	int runtime;
    int remainingT;
} pData;



typedef struct {
    pData *nodes;
    int len;
    int size;
} priority_heap_t;

void initializePQueue(priority_heap_t *Q){
		Q->len = 0;
		Q->size = 0;
		Q->nodes = NULL;
}

void push (priority_heap_t *h, pData data) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (pData *)realloc(h->nodes, h->size * sizeof (pData));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > data.priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = data.priority;
    h->nodes[i].pid = data.pid;
    h->nodes[i].arrival = data.arrival;
    h->nodes[i].runtime = data.runtime;

    h->len++;
}
pData pop (priority_heap_t *h) {
    int i, j, k;
    if (!h->len) {
        return;
    }
    pData data;
    data.priority = h->nodes[1].priority;
    data.pid = h->nodes[1].pid;
    data.arrival = h->nodes[1].arrival;
    data.runtime = h->nodes[1].runtime;

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

int getLength(priority_heap_t *h){
  return h->len;
}

/*int main () {
    priority_heap_t *h = (priority_heap_t *)calloc(1, sizeof (priority_heap_t));
    Data p1,p2,p3;
    p1.id = 25;
    p1.priority = 3;
    p2.id =123;
    p2.priority = 5;
    p3.id = 155;
    p3.priority = 9;

    push(h, p2);
    push(h, p3);
    push(h, p1);
    int i;
    for (i = 0; i < 3; i++) {
      Data *p = (Data *)calloc(1, sizeof(Data));
      p = pop(h);
        printf("id %d  priority %d\n", p->id,p->priority);
    }
    return 0;
}
*/
