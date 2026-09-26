#include <sys/stat.h>

#include <bits/getopt_core.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dir.h"
#include "handlefile.h"
#include "search.h"

static bool
is_directory(const char *path)
{
	struct stat st;
	if (stat(path, &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
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
	bool		     searchDirs	    = false;
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
	    (opt = getopt_long(argc, argv, "orE:", long_options, NULL)) != -1) {
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
		case 'r':
			searchDirs = true;
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
	int  MAX_LEN = 4096;
	char input_buffer[MAX_LEN];
	if (optind < argc) {
		bool matchingFound = false;
		fprintf(stderr, "Val is %d\n", argc - optind);
		bool show_prefix = (argc - optind > 1) || searchDirs;
		for (int i = optind; i < argc; ++i) {
			// same logic as before, just moved:
			// directory -> walk it, file -> search it
			if (searchDirs && is_directory(argv[i])) {
				if (scan_directory(argv[i], pattern,
					only_matching, showColory, 0))
					matchingFound = true;
				continue;
			}
			if (handle_file(argv[i], pattern, only_matching,
				showColory, show_prefix))
				matchingFound = true;
		}
		return matchingFound ? 0 : 1;
	} else {
		bool found = false;
		while (fgets(input_buffer, MAX_LEN, stdin)) {
			input_buffer[strcspn(input_buffer, "\n")] = 0;
			if (search_pattern(input_buffer, pattern, only_matching,
				showColory, false, NULL))
				found = true;
		}
		return found ? 0 : 1;
	}
	return 1;
}
