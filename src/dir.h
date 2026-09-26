#ifndef DIR_H
#define DIR_H

#include <stdbool.h>

bool scan_directory(const char *folder, const char *pattern, bool only_matching,
    bool showColory, int cnt);

#endif
