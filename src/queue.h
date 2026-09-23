#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
struct Queue {
	int  head;
	int  tail;
	int *arr;
};
// use pointers if cant be made local
int  queue_pop(struct Queue *q);
void queue_push(struct Queue *q, int val);
bool queue_isEmpty(struct Queue *q);

#endif
