#include "queue.h"
// queue implementation very easy one took help from google
int
queue_pop(struct Queue *q)
{
	if (q->head == q->tail)
		return -1;
	int returnVal = q->arr[q->head];
	q->head += 1;
	return returnVal;
}

void
queue_push(struct Queue *q, int val)
{
	q->arr[q->tail] = val;
	q->tail += 1;
}

bool
queue_isEmpty(struct Queue *q)
{
	return q->head == q->tail;
}
