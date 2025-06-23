#include "dv.h"
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

Dir* opendirectory(const char* dir) {
    DIR* dirp;
    Dir* d;

    if ((dirp = opendir(dir)) == NULL) {
        return NULL;
    }

    if ((d = (Dir*)malloc(sizeof(Dir))) == NULL) {
        closedir(dirp);
        return NULL;
    }

    d->dirp = dirp;
    return d;
}

DirEntry* readdirectory(Dir* d) {
    struct dirent* entry;

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

        return &d->de;
    }

    return NULL;
}

void closedirectory(Dir* d) {
    if (d) {
        if (d->dirp) {
            closedir(d->dirp);
        }
        free(d);
    }
}

void fsize(const char* name) {
    struct stat sbuf;

    if (stat(name, &sbuf) == -1) {
        fprintf(stderr, "fsize: cannot access %s: %s\n", name, strerror(errno));
        return;
    }

    if (S_ISDIR(sbuf.st_mode)) {
        dirwalk(name, fsize);
    }

    printf("%8ld %s\n", (long)sbuf.st_size, name);
}

void print(const char* name) {
    struct stat sbuf;

    if (stat(name, &sbuf) == -1) {
        fprintf(stderr, "print: cannot access %s: %s\n", name, strerror(errno));
        return;
    }

    if (S_ISDIR(sbuf.st_mode)) {
        dirwalk(name, print);
    }

    printf("%s\n", name);
}

void dirwalk(const char* dir, void (*func)(const char*)) {
    char      name[MAX_NAME];
    DirEntry* de;
    Dir*      d;

    if ((d = opendirectory(dir)) == NULL) {
        fprintf(stderr, "dirwalk: can't open %s: %s\n", dir, strerror(errno));
        return;
    }

    while ((de = readdirectory(d)) != NULL) {
        if (strcmp(de->name, ".") == 0 || strcmp(de->name, "..") == 0) {
            continue;
        }

        size_t dirlen = strlen(dir);
        int    result;

        if (dirlen > 0 && dir[dirlen - 1] == '/') {
            result = snprintf(name, sizeof(name), "%s%s", dir, de->name);
        } else {
            result = snprintf(name, sizeof(name), "%s/%s", dir, de->name);
        }

        if (result >= (int)sizeof(name)) {
            fprintf(
                stderr, "dirwalk: path too long \"%s/%s\"\n", dir, de->name
            );

            continue;
        }

        (*func)(name);
    }

    closedirectory(d);
}
