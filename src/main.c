#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

static void
emit_only_match(const char *p, int plen, bool show_color)
{
	if (show_color) {
		printf("\033[1;31m%.*s\033[0m\n", plen, p);
	} else {
		printf("%.*s\n", plen, p);
	}
}

void
print_match(const char *input, const char *p, int plen, bool show_color)
{
	int prefix_len = (int)(p - input);
	if (show_color) {
		// 1. Print everything before the match normally.
		printf("%.*s", prefix_len, input);

		// 2. Print exactly plen characters in bold red.
		printf("\033[1;31m%.*s\033[0m", plen, p);

		// 3. Print everything after the match normally.
		printf("%s\n", p + plen);
	} else {
		printf("%s\n", input);
	}
}

// Approach used for matching and printing:
//
// 1. The ERE pattern is compiled once into an NFA (tokens -> concat ->
//    postfix -> Thompson). `handleInput(p, nfa, endAnchor)` tries to match
//    *starting at p* and returns the number of bytes consumed (`plen`),
//    or -1 when nothing matches there. `endAnchor` enforces `$` by only
//    accepting when the match ends at end-of-line.
// 2. Input is split into lines so one stdin blob with `\n` behaves like
//    grep reading line by line.
// 3. Per line there are only two cases:
//    a) Anchored (`^...`): try exactly once at `input[0]`.
//    b) Unanchored: slide the start pointer `p` left to right.
//       - Normal mode prints the whole line once and stops at the first
//         hit (`break`).
//       - `-o` (only-matching) mode prints every non-overlapping hit and
//         keeps scanning (`continue` past the match).
// 4. Zero-length matches (`plen == 0`) advance by exactly one char so the
//    scan always makes progress and can never infinite-loop.
bool
search_pattern(const char *input_buffer, const char *pattern,
    bool only_matching, bool showColory)
{
	struct token_list t	   = returnTokens(pattern);
	t			   = insertConcats(t);
	t			   = convertIntoPost(t);
	struct NfaInfo nfa	   = thompson(t);
	char	     **handle	   = handleNewLine(input_buffer);
	// Hoisted anchor flags: same for every line/position.
	bool	       startAnchor = (pattern[0] == '^');
	bool	       endAnchor   = (strchr(pattern, '$') != NULL);
	int	       i	   = 0;
	bool	       final	   = false;
	while (handle[i] != NULL) {
		char *input = handle[i];
		// Case (a): anchored pattern, single attempt at line start.
		// i++ continue used because pattern can be \n\n ones so
		if (startAnchor) {
			int plen = handleInput(input, &nfa, endAnchor);
			if (plen == -1) {
				i++;
				continue;
			}
			final = true;
			if (only_matching) {
				emit_only_match(input, plen, showColory);
			} else {
				print_match(input, input, plen, showColory);
			}
			i++;
			continue;
		}
		// Case (b): unanchored, slide `p` over each start position.
		for (const char *p = input;; ++p) {
			int plen = handleInput(p, &nfa, endAnchor);
			if (plen == -1) {
				if (*p == '\0') {
					break;
				}
				continue;
			}
			final = true;
			if (only_matching) {
				emit_only_match(p, plen, showColory);
				if (*p == '\0') {
					break;
				}
				// Skip past this match so next search starts
				// after it. The loop's `++p` supplies one
				// step, hence `plen - 1` here. Empty matches
				// rely on `++p` alone to move one char.
				if (plen > 0) {
					p += plen - 1;
				}
				if (*p == '\0') {
					break;
				}
				continue;
			} else {
				print_match(input, p, plen, showColory);
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
	int		     opt;
	bool		     only_matching = false;
	const char	    *pattern;
	char		    *color	    = NULL;
	// using getopt took reference from other implementation
	// https://github.com/hlwqds/codecrafters-grep-c/blob/0d195d67f1258a5b9ab11d2f29ac2e9f2c1eef87/src/main.c
	static struct option long_options[] = {
		{ "color", required_argument, 0, 'c' }, { 0, 0, 0, 0 }
		// by returning 'c' i can decide
		// what happens is that getopt_long looks at long_options array
		// ./ans --color=always -E '\d'
		// it finds color and has required argument so "always" is
		// stored in optarg and returns 'c' which we stored in opt
	};
	while (
	    (opt = getopt_long(argc, argv, "oE:", long_options, NULL)) != -1) {
		// in this oE: o has no value & E needs value
		// so opt is E and opt arg is coming after pattern
		switch (opt) {
		case 'o':
			only_matching = true;
			break;
		case 'E':
			pattern = optarg;
			break;
		case 'c':
			color = optarg;
			break;
		default:
			break;
		}
	}
	bool showColory = false;
	if (color != NULL)
		showColory = true;
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
	if (search_pattern(input_buffer, pattern, only_matching, showColory)) {
		return 0;
	} else {
		return 1;
	}
}
