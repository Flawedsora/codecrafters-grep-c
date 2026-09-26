#include <sys/stat.h>

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dir.h"
#include "handlefile.h"

bool
scan_directory(const char *folder, const char *pattern, bool only_matching,
    bool showColory, int cnt)
{
	DIR *dir = opendir(folder);
	if (dir == NULL)
		return false;

	bool	       found = false;
	struct dirent *entry;
	char	       full_path[1024];
	struct stat    path_stat;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 ||
		    strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		if (cnt == 0) {
			snprintf(full_path, sizeof(full_path), "%s%s", folder,
			    entry->d_name);
		} else {
			snprintf(full_path, sizeof(full_path), "%s/%s", folder,
			    entry->d_name);
		}
		if (stat(full_path, &path_stat) != 0)
			continue;
		if (S_ISREG(path_stat.st_mode)) {
			if (handle_file(full_path, pattern, only_matching,
				showColory, true))
				found = true;
		} else if (S_ISDIR(path_stat.st_mode)) {
			if (scan_directory(full_path, pattern, only_matching,
				showColory, cnt + 1))
				found = true;
		}
	}
	closedir(dir);
	return found;
}
