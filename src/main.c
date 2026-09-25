#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "nfa.h"

static void
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
    bool only_matching, bool showColory)
{
	struct token_list t	   = returnTokens(pattern);
	t			   = insertConcats(t);
	t			   = convertIntoPost(t);
	struct NfaInfo nfa	   = thompson(t);
	// Hoisted anchor flags: same for every line/position.
	bool	       startAnchor = (pattern[0] == '^');
	bool	       endAnchor   = (strchr(pattern, '$') != NULL);
	char	       bufferAns[1024];
	int	       idx   = 0;
	char	      *c     = input_buffer;
	bool	       found = false;
	if (startAnchor) {
		int mlen = handleInput(c, &nfa, endAnchor);
		if (mlen != -1) {
			found = true;
			if (only_matching) {
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
		printf("%s\n", bufferAns);
	}
	return found;
}

int
main(int argc, char *argv[])
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging,
	// they'll be visible when running tests.
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
		// what happens is that getopt_long looks at
		// long_options array
		// ./ans --color=always -E '\d'
		// it finds color and has required argument so "always"
		// is stored in optarg and returns 'c' which we stored
		// in opt
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
	if (color != NULL) {
		if (strcmp(color, "always") == 0)
			showColory = true;
		else if (strcmp(color, "never") == 0)
			showColory = false;
		else if (isatty(1))
			showColory = true;
	}
	int   MAX_LEN = 4096;
	char  input_buffer[MAX_LEN];
	void *input_taker;
	if (optind < argc) {
		const char *filename = argv[optind];
		FILE	   *file     = fopen(filename, "r");
		if (file == NULL) {
			fprintf(stderr, "CANNOT OPEN FILE.\n");
			return 1;
		}
		input_taker = file;
	} else
		input_taker = stdin;
	bool returnVal = false;
	// no filename read stdin
	while (fgets(input_buffer, MAX_LEN, input_taker)) {
		input_buffer[strcspn(input_buffer, "\n")] = 0;
		// pass this buffer directly to search_pattern fxn
		fprintf(stderr, "Value of input buffer is: %s", input_buffer);
		if (search_pattern(input_buffer, pattern, only_matching,
			showColory))
			returnVal = true;
	}
	// Remove trailing newline
	if (returnVal) {
		return 0;
	} else {
		return 1;
	}
}
