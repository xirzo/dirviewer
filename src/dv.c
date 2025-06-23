#include "dv.h"
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void dirview(const char *path) {
    DirQueueEntry *q = malloc(MAX_QUEUE_SIZE * sizeof(DirQueueEntry));

    if (!q) {
        fprintf(stderr, "dirview: failed to allocate memory for queue\n");
        return;
    }

    int front = 0;
    int back = 0;

    q[back] = (DirQueueEntry){
        .depth = 0,
    };

    strncpy(q[back].path, path, sizeof(q[back].path) - 1);
    q[back].path[sizeof(q[back].path) - 1] = '\0';

    const char *filename;
    char       *last_slash = strrchr(path, '/');

    if (last_slash == NULL) {
        filename = path;
    } else if (*(last_slash + 1) == '\0') {
        char temp_path[PATH_MAX];
        strncpy(temp_path, path, sizeof(temp_path) - 1);
        temp_path[sizeof(temp_path) - 1] = '\0';

        size_t len = strlen(temp_path);
        if (len > 0 && temp_path[len - 1] == '/') {
            temp_path[len - 1] = '\0';
        }

        char *prev_slash = strrchr(temp_path, '/');
        filename = prev_slash ? prev_slash + 1 : temp_path;
    } else {
        filename = last_slash + 1;
    }

    back++;

    printf("%s\n", filename);

    while (front < back) {
        DirQueueEntry *current = &q[front];
        front++;

        for (int i = 0; i < current->depth; i++) {
            printf("\t");
        }

        if (strlen(current->file_name) != 0) {
            printf("%s\n", current->file_name);
        }

        struct stat sbuf;

        if (stat(current->path, &sbuf) == -1) {
            continue;
        }

        if (!S_ISDIR(sbuf.st_mode)) {
            continue;
        }

        Dir *dir = opendirectory(current->path);

        if (dir == NULL) {
            continue;
        }

        DirEntry *de;

        while ((de = readdirectory(dir)) != NULL) {
            if (strcmp(de->name, ".") == 0 || strcmp(de->name, "..") == 0) {
                continue;
            }

            DirQueueEntry child = { .depth = current->depth + 1 };

            size_t dirlen = strlen(current->path);
            int    result;

            if (dirlen > 0 && current->path[dirlen - 1] == '/') {
                result = snprintf(
                    child.path,
                    sizeof(child.path),
                    "%s%s",
                    current->path,
                    de->name
                );
            } else {
                result = snprintf(
                    child.path,
                    sizeof(child.path),
                    "%s/%s",
                    current->path,
                    de->name
                );
            }

            if (result >= (int)sizeof(child.path)) {
                fprintf(
                    stderr,
                    "dirview: path too long \"%s/%s\"\n",
                    current->path,
                    de->name
                );
                continue;
            }

            strncpy(
                child.file_name, de->file_name, sizeof(child.file_name) - 1
            );
            child.file_name[sizeof(child.file_name) - 1] = '\0';

            if (back < MAX_QUEUE_SIZE) {
                q[back++] = child;
            } else {
                fprintf(stderr, "dirview: queue is full, stopping bfs\n");
                closedirectory(dir);
                free(q);
                return;
            }
        }

        closedirectory(dir);
    }

    free(q);
}
