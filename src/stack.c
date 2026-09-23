#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

struct Stack {
	item_type *array;
	int	   top;
	int	   capacity;
};

static struct Stack *
createStack(int max_size)
{
	if (max_size <= 0) {
		fprintf(stderr, "Wrong stack size: %d", max_size);
		abort();
	}
	struct Stack *s = (struct Stack *)malloc(sizeof(struct Stack));
	if (s == NULL) {
		fprintf(stderr, "Insufficient memory\n");
		abort();
	}
	// now inside stack
	item_type *arr = (item_type *)malloc(sizeof(item_type) * max_size);
	if (arr == NULL) {
		fprintf(stderr, "Insufficient memory to initialize stack");
		abort();
	}
	s->array    = arr;
	s->capacity = max_size;
	s->top	    = -1;
	return s;
}

struct Stack *
create_stack()
{
	return createStack(256);
}

static void
stack_double(struct Stack *s)
{
	if (s == NULL) {
		fprintf(stderr, "Cannot double stack size.\n");
		abort();
	}
	int	   new_stack_size = 2 * s->capacity;
	item_type *new_array	  = (item_type *)malloc(
	    sizeof(item_type) * new_stack_size);
	if (new_array == NULL) {
		fprintf(stderr, "Insufficient memory to double stack.\n");
		abort();
	}
	for (int i = 0; i <= s->top; ++i)
		new_array[i] = s->array[i];
	free(s->array);
	s->array    = new_array;
	s->capacity = new_stack_size;
}

void
stack_push(struct Stack *s, item_type elem)
{
	if (s->top >= s->capacity - 1)
		stack_double(s);
	s->array[++s->top] = elem;
}

void
stack_pop(struct Stack *s)
{
	if (stack_is_empty(s)) {
		fprintf(stderr, "stack is empty can't pop.\n");
		abort();
	}
	s->top--;
}

item_type
stack_top(struct Stack *s)
{
	if (stack_is_empty(s)) {
		fprintf(stderr, "Stack is empty can't find top.\n");
		abort();
	}
	return s->array[s->top];
}

int
stack_is_empty(struct Stack *s)
{
	if (s == NULL) {
		fprintf(stderr, "cannot work with null stack.\n");
		abort();
	}
	return s->top < 0;
}

int
stack_size(struct Stack *s)
{
	if (s == NULL) {
		fprintf(stderr, "cannot work with null stack.\n");
		abort();
	}
	return s->top + 1;
}

void
stack_clear(struct Stack *s)
{
	if (s == NULL) {
		fprintf(stderr, "cannot work with null stack.\n");
		abort();
	}
	s->top = -1;
}

void
destroy_stack(struct Stack *s)
{
	if (s == NULL)
		return;
	free(s->array);
	free(s);
}
