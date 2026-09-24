#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "nfa.h"

const char *pattern;
char **
handleNewLine(const char *input_line)
{
	int   len  = (int)strlen(input_line);
	char *dest = (char *)malloc(len + 2);
	if (dest == NULL)
		return NULL;
	strcpy(dest, input_line);
	dest[len]     = '\n';
	dest[len + 1] = '\0';
	char **fres   = (char **)calloc(100, sizeof(char *));
	int    idx    = 0;
	int    fidx   = 0;
	while (idx < (int)strlen(dest)) {
		int ilen = 0, cidx = idx;
		while (cidx < (int)strlen(dest) && dest[cidx] != '\n')
			cidx++, ilen++;
		char *allocated = (char *)malloc(ilen + 1);
		if (allocated == NULL)
			return NULL;
		int aidx = 0;
		while (idx < (int)strlen(dest) && dest[idx] != '\n')
			allocated[aidx++] = dest[idx++];
		allocated[aidx] = '\0';
		fres[fidx++]	= allocated;
		if (dest[idx] == '\n')
			idx++;
	}
	fres[fidx] = NULL;
	free(dest);
	return fres;
}

bool
search_pattern(const char *input_buffer, const char *pattern,
    bool only_matching)
{
	struct token_list t   = returnTokens(pattern);
	t		      = insertConcats(t);
	t		      = convertIntoPost(t);
	struct NfaInfo nfa    = thompson(t);
	char	     **handle = handleNewLine(input_buffer);
	int	       i      = 0;
	bool	       final  = false;
	while (handle[i] != NULL) {
		char *input = handle[i];
		// each input is nwe line
		if (pattern[0] == '^') {
			int pos = handleInput(input, &nfa,
			    strchr(pattern, '$') != NULL);
			if (pos != -1)
				final = true;
			if (only_matching && pos != -1) {
				char *curr = input;
				char *res  = malloc(pos + 1);
				for (int j = 0; j < pos; ++j)
					res[j] = curr[j];
				res[pos] = '\0';
				printf("%s\n", res);
				free(res);
			} else if (pos != -1) {
				printf("%s\n", input);
			}
		} else {
			for (const char *p = input;; ++p) {
				int pos = handleInput(p, &nfa,
				    strchr(pattern, '$') != NULL);
				if (pos != -1)
					final = true;
				if (only_matching && pos != -1) {
					// -o flag to just get that slice which
					// satisfies the pattern
					char *curr = (char *)p;
					char *res  = malloc(pos + 1);
					for (int j = 0; j < pos; ++j)
						res[j] = curr[j];
					res[pos] = '\0';
					printf("%s\n", res);
					free(res);
				} else if (pos != -1) {
					printf("%s\n",
					    input); // print input and exit in
					break;
				}
				if (*p == '\0')
					break;
			}
		}
		i++;
	}
	return final;
}

int
main(int argc, char *argv[])
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be
	// visible when running tests.
	fprintf(stderr, "Logs from your program will appear here\n");
	int	    opt;
	bool	    only_matching = false;
	const char *pattern;
	// using getopt took reference from other implementation
	// https://github.com/hlwqds/codecrafters-grep-c/blob/0d195d67f1258a5b9ab11d2f29ac2e9f2c1eef87/src/main.c
	while ((opt = getopt(argc, argv, "oE:")) != -1) {
		switch (opt) {
		case 'o':
			only_matching = true;
			break;
		case 'E':
			pattern = optarg;
			break;
		default:
			break;
		}
	}

	char   input_buffer[4096];
	int    c;
	size_t total = 0;
	while ((c = getchar()) != EOF) {
		if (total + 1 < sizeof(input_buffer))
			input_buffer[total++] = (char)c;
	}
	if (total == 0)
		return 1;
	input_buffer[total] = '\0';
	// Remove trailing newline
	if (search_pattern(input_buffer, pattern, only_matching)) {
		return 0;
	} else {
		return 1;
	}
}
