#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "handlefile.h"
#include "search.h"
bool
handle_file(const char *path, const char *pattern, bool only_matching,
    bool showColory, bool show_prefix)
{
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "CANNOT OPEN CURRENT FILE.\n");
		return false;
	}
	char input[4096];
	bool found = false;
	while (fgets(input, sizeof(input), f)) {
		input[strcspn(input, "\n")] = 0;
		if (search_pattern(input, pattern, only_matching, showColory,
			show_prefix, path))
			found = true;
	}
	fclose(f);
	return found;
}
