#include <stdbool.h>
#ifndef TOKENIZE_H
#define TOKENIZE_H

enum tokenType {
	TOKEN_CHAR,
	TOKEN_DIGIT,
	TOKEN_WORD,
	TOKEN_INCLUSION,
	TOKEN_EXCLUSION,
	TOKEN_PLUS,
	TOKEN_PRODUCT,
	TOKEN_POWER,
	TOKEN_QUESTION,
	TOKEN_CAP,
	TOKEN_DOLLAR,
	TOKEN_ALTERATION, // it is or |
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_ANY,
	TOKEN_CONCAT,
	TOKEN_EPSILON, // this added extra for epsilon transition
};

struct token {
	enum tokenType type;
	char	       curr;
	char	      *group; // this is for inclusion exclusion
};

struct token_list {
	struct token *tokens;
	int	      length;
};

struct token_list returnTokens(const char *patt);

#endif
