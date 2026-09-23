#include <stdio.h>
#include <stdlib.h>

#include "thomStack.h"

struct thomStack *
createthomStack(int max_size)
{
	if (max_size <= 0) {
		fprintf(stderr, "Wrong stack size: %d", max_size);
		abort();
	}
	struct thomStack *st = (struct thomStack *)malloc(
	    sizeof(struct thomStack));
	if (st == NULL) {
		fprintf(stderr, "Stack not created.\n");
		abort();
	}
	// now inner you declared this in thomStack.h here actual allocation
	struct Stateinfo **arr = (struct Stateinfo **)malloc(
	    sizeof(struct Stateinfo *) * max_size);
	st->graph    = arr;
	st->capacity = max_size;
	st->top	     = -1;
	return st;
}

struct thomStack *
createthom_stack()
{
	return createthomStack(256);
}

bool
isEmpty(struct thomStack *st)
{
	if (st == NULL) {
		fprintf(stderr, "cannot work with empty stack.\n");
		abort();
	}
	return st->top < 0;
}

static void
stack_double(struct thomStack *st)
{
	if (st == NULL) {
		fprintf(stderr, "cannot work with empty stack.\n");
		abort();
	}
	int		   new_stack_size = 2 * st->capacity;
	struct Stateinfo **new_array	  = (struct Stateinfo **)malloc(
	    new_stack_size * sizeof(struct Stateinfo *));
	if (new_array == NULL) {
		fprintf(stderr, "cannot work with empty stack.\n");
		abort();
	}
	for (int i = 0; i <= st->top; ++i)
		new_array[i] = st->graph[i];
	free(st->graph);
	// new array is the new graph
	st->graph    = new_array;
	st->capacity = new_stack_size;
}

void
push(struct thomStack *st, struct Stateinfo *graph)
{
	if (st->top >= st->capacity - 1)
		stack_double(st);
	st->graph[++st->top] = graph;
}

struct Stateinfo *
pop(struct thomStack *st)
{
	if (st == NULL)
		abort();
	if (isEmpty(st)) {
		fprintf(stderr, "stack is empty can't pop.\n");
		abort();
	}
	return st->graph[st->top--];
}

struct Stateinfo *
top(struct thomStack *st)
{
	if (isEmpty(st)) {
		fprintf(stderr, "stack is empty can't pop.\n");
		abort();
	}
	return st->graph[st->top];
}

void
destroyStack(struct thomStack *st)
{
	if (st == NULL)
		return;
	free(st->graph);
	free(st);
}
