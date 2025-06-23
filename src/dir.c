#include "dir.h"
#include <stdlib.h>
#include <string.h>

Dir *opendirectory(const char *dir) {
    DIR *dirp;
    Dir *d;

    if ((dirp = opendir(dir)) == NULL) {
        return NULL;
    }

    if ((d = (Dir *)malloc(sizeof(Dir))) == NULL) {
        closedir(dirp);
        return NULL;
    }

    d->dirp = dirp;
    return d;
}

DirEntry *readdirectory(Dir *d) {
    struct dirent *entry;

    if (d == NULL || d->dirp == NULL) {
        return NULL;
    }

    while ((entry = readdir(d->dirp)) != NULL) {
        if (entry->d_ino == 0) {
            continue;
        }

        d->de.inode = entry->d_ino;

        size_t name_len = strlen(entry->d_name);

        if (name_len > NAME_MAX) {
            name_len = NAME_MAX;
        }

        strncpy(d->de.name, entry->d_name, name_len);
        d->de.name[name_len] = '\0';

        char *slash = strrchr(entry->d_name, '/');

        if (slash == NULL) {
            strncpy(d->de.file_name, entry->d_name, name_len);
            d->de.file_name[name_len] = '\0';
        } else {
            strncpy(d->de.file_name, entry->d_name + *slash, name_len - *slash);
            d->de.file_name[name_len - *slash] = '\0';
        }

        return &d->de;
    }

    return NULL;
}

void closedirectory(Dir *d) {
    if (d) {
        if (d->dirp) {
            closedir(d->dirp);
        }
        free(d);
    }
}
