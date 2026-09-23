#ifndef _STACK_H
#define _STACK_H

typedef char item_type;

struct Stack *create_stack();
void	      destroy_stack(struct Stack *s);
void	      stack_push(struct Stack *s, item_type elem);
void	      stack_pop(struct Stack *s);
item_type     stack_top(struct Stack *s);
int	      stack_is_empty(struct Stack *s);
int	      stack_size(struct Stack *s);
void	      stack_clear(struct Stack *s);

#endif
