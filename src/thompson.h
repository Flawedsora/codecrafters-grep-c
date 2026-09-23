#ifndef THOMPSON_H
#define THOMPSON_H
#include <stdbool.h>
#include <stdlib.h>

#include "shunting.h"
struct Node {
	struct token label;
	int	     dest;
	struct Node *next;
};

struct Stateinfo {
	int start;
	int dest;
};

struct Graph {
	int	      V;
	struct Node **adj;
};

struct NfaInfo {
	// nfa has information of graph start and end states
	struct Graph *g;
	int	      start;
	int	      accept;
};

static struct token EPS = {
	.type  = TOKEN_EPSILON,
	.curr  = '\0',
	.group = NULL,
};
struct NfaInfo thompson(struct token_list postFix);

#endif
