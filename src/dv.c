#include "dv.h"
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void dirview(const char *path) {
    DirStackEntry *stack = malloc(MAX_QUEUE_SIZE * sizeof(DirStackEntry));

    if (!stack) {
        fprintf(stderr, "dirview: failed to allocate memory for stack\n");
        return;
    }

    int top = 0;

    stack[top] = (DirStackEntry){
        .depth = 0,
        .is_last = 1,
    };

    strncpy(stack[top].path, path, sizeof(stack[top].path) - 1);
    stack[top].path[sizeof(stack[top].path) - 1] = '\0';

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

    strncpy(stack[top].file_name, filename, sizeof(stack[top].file_name) - 1);
    stack[top].file_name[sizeof(stack[top].file_name) - 1] = '\0';
    top++;

    int has_more_siblings[100] = { 0 };

    while (top > 0) {
        top--;
        DirStackEntry *current = &stack[top];

        if (current->depth == 0) {
            printf("%s\n", current->file_name);
        } else {
            for (int i = 0; i < current->depth - 1; i++) {
                if (has_more_siblings[i]) {
                    printf("│   ");
                } else {
                    printf("    ");
                }
            }

            if (current->is_last) {
                printf("└── ");
                has_more_siblings[current->depth - 1] = 0;
            } else {
                printf("├── ");
                has_more_siblings[current->depth - 1] = 1;
            }

            struct stat sbuf;

            if (stat(current->path, &sbuf) != -1 && S_ISDIR(sbuf.st_mode)) {
                printf("%s\n", current->file_name);
            } else {
                printf("%s\n", current->file_name);
            }
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

        DirStackEntry children[1000];
        DirEntry     *de;
        int           child_count = 0;

        while ((de = readdirectory(dir)) != NULL && child_count < 1000) {
            if (strcmp(de->name, ".") == 0 || strcmp(de->name, "..") == 0) {
                continue;
            }

            DirStackEntry child = { .depth = current->depth + 1 };

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
            children[child_count++] = child;
        }

        closedirectory(dir);

        for (int i = 0; i < child_count - 1; i++) {
            for (int j = i + 1; j < child_count; j++) {
                if (strcmp(children[i].file_name, children[j].file_name) > 0) {
                    DirStackEntry temp = children[i];
                    children[i] = children[j];
                    children[j] = temp;
                }
            }
        }

        for (int i = child_count - 1; i >= 0; i--) {
            children[i].is_last = (i == child_count - 1);

            if (top < MAX_QUEUE_SIZE) {
                stack[top++] = children[i];
            } else {
                fprintf(stderr, "dirview: stack is full, stopping dfs\n");
                free(stack);
                return;
            }
        }
    }

    free(stack);
}
