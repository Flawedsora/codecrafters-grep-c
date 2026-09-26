#ifndef HANDLEFILE_H
#define HANDLEFILE_H

#include <stdbool.h>

bool handle_file(const char *path, const char *pattern, bool only_matching,
    bool showColory, bool show_prefix);

#endif
