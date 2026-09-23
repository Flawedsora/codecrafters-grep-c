#include <stdlib.h>
#include <string.h>
// Thompson construction
// implement each fragment
// Each fragment acts as a graph

#include "thomStack.h"
#include "tokenize.h"

struct Graph *
newGraph(int N)
{
	// while creation start from outer allocation n is pointer to the graph
	struct Graph *g = malloc(sizeof(struct Graph));
	if (!g)
		return NULL;
	g->V   = N;
	g->adj = calloc(N, sizeof(struct Node *));
	if (!g->adj) {
		free(g);
		return NULL;
	}
	return g;
}

void
addEdge(struct Graph *g, int s, int d, struct token label)
{
	// prepend d to the adjacency list TQ GEMINI
	struct Node *newNode = malloc(sizeof(struct Node));
	if (!newNode)
		return;
	newNode->label = label;
	newNode->dest  = d;
	newNode->next  = g->adj[s];
	g->adj[s]      = newNode;
}

// symbol of input conversion
// https://en.wikipedia.org/wiki/Thompson%27s_construction
// https://medium.com/swlh/visualizing-thompsons-construction-algorithm-for-nfas-step-by-step-f92ef378581b
static int global_cnt = 0;

struct Stateinfo *
symbolConversion(struct Graph *g, struct token label)
{
	int st = global_cnt, dst = global_cnt + 1;
	addEdge(g, st, dst, label);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = st;
	info->dest  = dst;
	global_cnt += 2;
	return info;
}

struct Stateinfo *
orCombination(struct Graph *g, struct Stateinfo *frag1, struct Stateinfo *frag2)
{
	int st = global_cnt, dst = global_cnt + 1;
	addEdge(g, st, frag1->start, EPS);
	addEdge(g, st, frag2->start, EPS);
	addEdge(g, frag1->dest, dst, EPS);
	addEdge(g, frag2->dest, dst, EPS);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = st;
	info->dest  = dst;
	global_cnt += 2;
	return info;
}

struct Stateinfo *
kleeneStar(struct Graph *g, struct Stateinfo *frag)
{
	int st = global_cnt, dst = global_cnt + 1;
	addEdge(g, st, frag->start, EPS);
	addEdge(g, frag->dest, dst, EPS);
	addEdge(g, frag->dest, frag->start, EPS);
	addEdge(g, st, dst, EPS);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = st;
	info->dest  = dst;
	global_cnt += 2;
	return info;
}

struct Stateinfo *
epsilonConversion(struct Graph *g)
{
	int st = global_cnt, dst = global_cnt + 1;
	addEdge(g, st, dst, EPS);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = st;
	info->dest  = dst;
	global_cnt += 2;
	return info;
}

struct Stateinfo *
concatConversion(struct Graph *g, struct Stateinfo *f1, struct Stateinfo *f2)
{
	addEdge(g, f1->dest, f2->start, EPS);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = f1->start;
	info->dest  = f2->dest;
	return info;
}
struct Stateinfo *
plusConversion(struct Graph *g, struct Stateinfo *f)
{
	// first we get
	// then we get star transforms
	struct Stateinfo *sPart = kleeneStar(g, f);
	// now do connection
	addEdge(g, f->dest, sPart->start, EPS);
	struct Stateinfo *info = (struct Stateinfo *)malloc(
	    sizeof(struct Stateinfo));
	if (!info)
		return NULL;
	info->start = f->start;
	info->dest  = sPart->dest;
	return info;
}

struct Stateinfo *
quesConversion(struct Graph *g, struct Stateinfo *f)
{
	// see eg if is see a in postfix then f(fragment) is 0-a-1 a is the
	// connection
	struct Stateinfo *eps = epsilonConversion(g);
	return orCombination(g, eps, f);
}

struct Nfa {
	struct Graph *g;
	int	      start;
	int	      accept;
};

// now to combine all see we have to convert postfix to nfa graph
// I want the pointer cant return directly because local pointer returning
// becomes invalid
struct NfaInfo
thompson(struct token_list postFix)
{
	global_cnt		 = 0;
	// stack of graphs
	int		  len	 = postFix.length;
	struct token	 *tokens = postFix.tokens;
	struct thomStack *S	 = createthomStack(len);
	struct Graph	 *g	 = newGraph(2 * len + 2);
	struct Stateinfo *frag1, *frag2;
	for (int i = 0; i < len; ++i) {
		struct token tokenVal = tokens[i];
		switch (tokenVal.type) {
		case TOKEN_CHAR:
		case TOKEN_DIGIT:
		case TOKEN_WORD:
		case TOKEN_INCLUSION:
		case TOKEN_EXCLUSION:
		case TOKEN_ANY:
			push(S, symbolConversion(g, tokenVal));
			break;
		case TOKEN_CONCAT:
			frag2 = pop(S);
			frag1 = pop(S);
			push(S, concatConversion(g, frag1, frag2));
			break;
		case TOKEN_ALTERATION:
			frag2 = pop(S);
			frag1 = pop(S);
			push(S, orCombination(g, frag1, frag2));
			break;
		case TOKEN_PRODUCT:
			push(S, kleeneStar(g, pop(S)));
			break;
		case TOKEN_PLUS:
			push(S, plusConversion(g, pop(S)));
			break;
		case TOKEN_QUESTION:
			push(S, quesConversion(g, pop(S)));
			break;
		case TOKEN_CAP:
		case TOKEN_DOLLAR:
			// zero-width anchors: epsilon fragment keeps the
			// fragment stack balanced; actual anchoring is done
			// by the caller (main.c)
			push(S, epsilonConversion(g));
			break;
		default:
			break;
		}
	}
	// if returning pointer wrap it in struct
	struct Stateinfo *result = top(S);
	int		  start	 = result->start;
	int		  accept = result->dest;
	free(result);
	return (struct NfaInfo) {
		.g	= g,
		.accept = accept,
		.start	= start,
	};
}
