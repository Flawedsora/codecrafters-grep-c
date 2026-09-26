#include <stdio.h>
#include <string.h>

#include "nfa.h"
#include "search.h"

void
emit_only_match(const char *p, int plen, bool show_color)
{
	if (show_color) {
		printf("\033[1;31m%.*s\033[0m\n", plen, p);
	} else {
		printf("%.*s\n", plen, p);
	}
}

bool
search_pattern(const char *input_buffer, const char *pattern,
    bool only_matching, bool showColory, bool show_prefix, const char *filename)
{
	struct token_list t	   = returnTokens(pattern);
	t			   = insertConcats(t);
	t			   = convertIntoPost(t);
	struct NfaInfo nfa	   = thompson(t);
	bool	       startAnchor = (pattern[0] == '^');
	bool	       endAnchor   = (strchr(pattern, '$') != NULL);
	char	       bufferAns[1024];
	int	       idx   = 0;
	const char    *c     = input_buffer;
	bool	       found = false;
	if (startAnchor) {
		int mlen = handleInput(c, &nfa, endAnchor);
		if (mlen != -1) {
			found = true;
			if (only_matching) {
				if (show_prefix)
					printf("%s:", filename);
				emit_only_match(c, mlen, showColory);
				return true;
			}
			const char *red	      = "\033[1;31m";
			const char *reset     = "\033[0m";
			size_t	    red_len   = strlen(red);
			size_t	    reset_len = strlen(reset);
			if (showColory) {
				memcpy(bufferAns + idx, red, red_len);
				idx += (int)red_len;
			}
			memcpy(bufferAns + idx, c, (size_t)mlen);
			idx += mlen, c += mlen;
			if (showColory) {
				memcpy(bufferAns + idx, reset, reset_len);
				idx += (int)reset_len;
			}
			while (*c != '\0')
				bufferAns[idx++] = *c, c++;
			bufferAns[idx] = '\0';
			if (show_prefix)
				printf("%s:", filename);

			printf("%s\n", bufferAns);
		}
		return found;
	}
	while (*c != '\0') {
		int mlen = handleInput(c, &nfa, endAnchor);
		if (mlen != -1) {
			found = true;
			if (only_matching) {
				emit_only_match(c, mlen, showColory);
				c += mlen;
				if (mlen == 0 && *c != '\0')
					c += 1;
				continue;
			}
			if (mlen == 0) {
				if (*c != '\0') {
					bufferAns[idx++] = *c;
					c += 1;
				}
				continue;
			}
			const char *red	      = "\033[1;31m";
			const char *reset     = "\033[0m";
			size_t	    red_len   = strlen(red);
			size_t	    reset_len = strlen(reset);
			if (showColory) {
				memcpy(bufferAns + idx, red, red_len);
				idx += (int)red_len;
			}
			memcpy(bufferAns + idx, c, (size_t)mlen);
			idx += mlen, c += mlen;
			if (showColory) {
				memcpy(bufferAns + idx, reset, reset_len);
				idx += (int)reset_len;
			}
		} else {
			if (!only_matching)
				bufferAns[idx++] = *c;
			c += 1;
		}
	}
	if (!only_matching && found) {
		bufferAns[idx] = '\0';
		if (show_prefix)
			printf("%s:", filename);

		printf("%s\n", bufferAns);
	}
	return found;
}
