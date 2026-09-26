#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenize.h"

// Pattern from user -> array of tokens

struct token list[100]; // array of type struct token

// we are returning struct -> array + length
struct token_list
returnTokens(const char *pattern)
{
	// TQ opencode
	// do it lookahead only
	// if you see \d we do normal
	// and also check if next is + then do that operation normally
	int list_idx = 0;
	while (*pattern != '\0') {
		struct token tok = {
			TOKEN_CHAR,
			*pattern,
			NULL,
		};
		if (*pattern == '\\' && *(pattern + 1) == 'd') {
			tok.type = TOKEN_DIGIT;
			tok.curr = ' ';
			pattern += 2;
		} else if (*pattern == '\\' && *(pattern + 1) == 'w') {
			tok.type = TOKEN_WORD;
			tok.curr = ' ';
			pattern += 2;
		} else if (*pattern == '^') {
			tok.type = TOKEN_CAP;
			tok.curr = ' ';
			pattern += 1;
		} else if (*pattern == '$') {
			if (*(pattern + 1) != '\0')
				exit(1);
			tok.type = TOKEN_DOLLAR;
			tok.curr = ' ';
			pattern += 1;
		} else if (*pattern == '[') {
			const char *start = pattern + 1;
			bool	    exl	  = false;
			int	    len	  = 0;
			if (*start == '^')
				exl = true, start++;
			const char *scan = start;
			while (*scan != '\0' && *scan != ']')
				len++, scan += 1;
			char *group = malloc(len + 1);
			if (group) {
				int g = 0;
				while (*start != '\0' && *start != ']')
					group[g++] = *start++;
				group[g] = '\0';
			}
			tok.type  = exl ? TOKEN_EXCLUSION : TOKEN_INCLUSION;
			tok.curr  = ' ';
			tok.group = group;
			pattern	  = (*scan == ']') ? scan + 1 : scan;
		} else if (*pattern == '|') {
			tok.type = TOKEN_ALTERATION;
			tok.curr = ' ';
			pattern += 1;
		} else if (*pattern == '(') {
			tok.type = TOKEN_LPAREN;
			tok.curr = ' ';
			pattern += 1;
		} else if (*pattern == ')') {
			tok.type = TOKEN_RPAREN;
			tok.curr = ' ';
			pattern += 1;
		} else if (*pattern == '.') {
			tok.type = TOKEN_ANY;
			tok.curr = ' ';
			pattern += 1;
		} else {
			tok.curr = *pattern;
			pattern += 1;
		} // see i have progressed enough now checking
		// BELOW ONES ARE AFTER ATTACKERS
		list[list_idx++] = tok;
		// made mistake here take the token and also below ones
		if (*(pattern) == '+') {
			tok.type = TOKEN_PLUS;
			tok.curr = ' ';
			pattern += 1;
			list[list_idx++] = tok;
		} else if (*(pattern) == '*') {
			tok.type = TOKEN_PRODUCT;
			tok.curr = ' ';
			pattern += 1;
			list[list_idx++] = tok;
		} else if (*(pattern) == '?') {
			tok.type = TOKEN_QUESTION;
			tok.curr = ' ';
			pattern += 1;
			list[list_idx++] = tok;
		}
		if (*pattern == '{') { // just extending previous approach
			int	     dig = 0;
			struct token pok = list[list_idx - 1];
			pattern += 1;
			while (isdigit(*pattern) && *pattern != '}' &&
			    *pattern != ',')
				dig = dig * 10 + (*pattern - '0'), pattern += 1;
			dig--;
			while (dig--)
				list[list_idx++] = pok;
			if (*pattern == ',') {
				list[list_idx++] = pok;
				// see actually eg: ca{2,t}t => caaa*t
				// I caa has been handled above and now i just
				// need to handle a*
				// we were adding one less previously because
				// a{2} while going from left to right we took a
				// if we considered 2 then it will be extra so
				// we did dig-- but here we want one more time
				// so
				struct token mok = list[list_idx - 1];
				mok.curr	 = ' ';
				mok.type =
				    TOKEN_PRODUCT; // everything else is
						   // borrowed from previous
				pattern += 1;
				list[list_idx++] = mok;
			}
			if (*pattern == '}')
				pattern += 1;
		}
	}
	return (struct token_list) { .tokens = list, .length = list_idx };
}
