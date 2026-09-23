#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nfa.h"

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
search_pattern(const char *input_buffer, const char *pattern)
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
			if (handleInput(input, &nfa,
				strchr(pattern, '$') != NULL)) {
				printf("%s\n", input);
				final = true;
			}
		} else {
			for (const char *p = input;; ++p) {
				if (handleInput(p, &nfa,
					strchr(pattern, '$') != NULL)) {
					printf("%s\n", input);
					final = true;
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

	if (argc != 3) {
		fprintf(stderr, "Expected two arguments\n");
		return 1;
	}
	const char *flag    = argv[1];
	const char *pattern = argv[2];

	if (strcmp(flag, "-E") != 0) {
		fprintf(stderr, "Expected first argument to be '-E'\n");
		return 1;
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
	if (search_pattern(input_buffer, pattern)) {
		return 0;
	} else {
		return 1;
	}
}
