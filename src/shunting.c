#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "tokenize.h"
// shunting yard algorithm
// https://mathcenter.oxford.emory.edu/site/cs171/shuntingYardAlgorithm/
// This is required because thompson construction requires postfix expressions

enum assoc { ASSOC_LEFT, ASSOC_RIGHT };

struct op_info {
	enum tokenType type;
	int	       prec;
	enum assoc     assoc;
};
// made static to change the lifetime
static const struct op_info ops[] = {
	{ TOKEN_ALTERATION, 1, ASSOC_LEFT },
	{ TOKEN_CONCAT, 2, ASSOC_LEFT },
	{ TOKEN_QUESTION, 3, ASSOC_LEFT },
	{ TOKEN_PRODUCT, 3, ASSOC_LEFT },
	{ TOKEN_PLUS, 3, ASSOC_LEFT },
};
static const struct op_info *
lookup(enum tokenType t)
{
	for (size_t i = 0; i < sizeof ops / sizeof(ops[0]); ++i) {
		if (ops[i].type == t)
			return &ops[i];
	}
	return NULL;
}

int
checkisOperator(enum tokenType t)
{
	return lookup(t) != NULL;
}

int
prec(enum tokenType t)
{
	const struct op_info *cl = lookup(t);
	return cl ? cl->prec : -1;
}

int
left_associative(enum tokenType t)
{
	const struct op_info *cl = lookup(t);
	return cl ? cl->assoc == ASSOC_LEFT : -1;
}

int
right_associative(enum tokenType t)
{
	const struct op_info *cl = lookup(t);
	return cl ? cl->assoc == ASSOC_RIGHT : -1;
}

struct token
make_type_op_token(enum tokenType t)
{
	return (struct token) { t, '\0', NULL };
}

struct token_list
convertIntoPost(struct token_list tokens)
{
	struct token *arr	= tokens.tokens;
	int	      token_len = tokens.length; // TODO will try later
	struct token *postfix = malloc(sizeof(struct token) * (token_len + 1));
	struct Stack *st      = create_stack();
	if (!postfix) {
		destroy_stack(st);
		return (struct token_list) { .tokens = NULL, .length = 0 };
	}
	int idx = 0, ansIdx = 0;
	while (idx < token_len) {
		enum tokenType cur = arr[idx].type;
		if (cur == TOKEN_LPAREN) {
			stack_push(st, (item_type)TOKEN_LPAREN);
		} else if (cur == TOKEN_RPAREN) {
			while (!stack_is_empty(st) &&
			    (enum tokenType)stack_top(st) != TOKEN_LPAREN) {
				postfix[ansIdx++] = make_type_op_token(
				    (enum tokenType)stack_top(st));
				stack_pop(st);
			}
			if (!stack_is_empty(st))
				stack_pop(st); /* discard '(' */
		} else if (checkisOperator(cur)) {
			if (stack_is_empty(st) ||
			    (enum tokenType)stack_top(st) == TOKEN_LPAREN ||
			    prec(cur) > prec((enum tokenType)stack_top(st)) ||
			    (prec(cur) == prec((enum tokenType)stack_top(st)) &&
				right_associative(cur))) { /* rule 4/5 */
				stack_push(st, (item_type)cur);
			} else { /* rule 6 */
				while (!stack_is_empty(st) &&
				    (enum tokenType)stack_top(st) !=
					TOKEN_LPAREN &&
				    (prec(cur) < prec((enum tokenType)stack_top(
						     st)) ||
					(prec(cur) ==
						prec((enum tokenType)stack_top(
						    st)) &&
					    left_associative(cur)))) {
					postfix[ansIdx++] = make_type_op_token(
					    (enum tokenType)stack_top(st));
					stack_pop(st);
				}
				stack_push(st, (item_type)cur);
			}
		} else { /* operand */
			postfix[ansIdx++] = arr[idx];
		}
		idx += 1;
	}
	while (!stack_is_empty(st)) {
		postfix[ansIdx++] = make_type_op_token(
		    (enum tokenType)stack_top(st));
		stack_pop(st);
	}
	destroy_stack(st); // took such parts from AI
	return (struct token_list) { .tokens = postfix, .length = ansIdx };
}

// NOW convert it by adding concats
int
is_operand(enum tokenType t)
{
	switch (t) {
	case TOKEN_CHAR:
	case TOKEN_DIGIT:
	case TOKEN_WORD:
	case TOKEN_INCLUSION:
	case TOKEN_EXCLUSION:
	case TOKEN_ANY:
	case TOKEN_CAP:
	case TOKEN_DOLLAR:
		return 1;
	default:
		return 0;
	}
}

int
is_atom_end(enum tokenType t)
{
	// check left
	return is_operand(t) || t == TOKEN_PLUS || t == TOKEN_PRODUCT ||
	    t == TOKEN_QUESTION || t == TOKEN_RPAREN;
}

int
is_atom_start(enum tokenType t)
{
	return is_operand(t) || t == TOKEN_LPAREN;
}

struct token_list
insertConcats(struct token_list infix)
{
	// ) . ( first is is_atom_end and second one is is_atom_start
	int	      len = infix.length;
	// alot of help from AI for allocations part and proper error handling
	struct token *out = malloc(sizeof(struct token) * (2 * len + 1));
	if (!out)
		return (struct token_list) { .tokens = NULL, .length = 0 };
	int o = 0;
	for (int i = 0; i < len; ++i) {
		if (i > 0 && is_atom_end(infix.tokens[i - 1].type) &&
		    is_atom_start(infix.tokens[i].type)) {
			struct token c = {
				TOKEN_CONCAT,
				'\0',
				NULL,
			};
			out[o++] = c;
		}
		out[o++] = infix.tokens[i];
	}
	return (struct token_list) { .tokens = out, .length = o };
}
