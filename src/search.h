#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>

void emit_only_match(const char *p, int plen, bool show_color);
bool search_pattern(const char *input_buffer, const char *pattern,
    bool only_matching, bool showColory, bool show_prefix,
    const char *filename);
#endif
