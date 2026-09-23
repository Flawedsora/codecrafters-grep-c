#include <stdbool.h>

#ifndef THOMPSON_STACK
#define THOMPSON_STACK
#include "thompson.h"

struct thomStack {
	int top;
	int capacity;
	// stack is array of pointers that represent graph
	// was having issues TQ AI
	struct Stateinfo *
	    *graph; // store array of pointers returned by thompson
};

struct thomStack *createthomStack(int max_size);

bool isEmpty(struct thomStack *st);

void push(struct thomStack *st, struct Stateinfo *graph);

struct Stateinfo *pop(struct thomStack *st);

struct Stateinfo *top(struct thomStack *st);

void destroyStack(struct thomStack *st);

#endif
