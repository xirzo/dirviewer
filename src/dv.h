#ifndef DV_H
#define DV_H

#include "dir.h"

#define MAX_QUEUE_SIZE 10000

typedef struct {
    char     path[PATH_MAX];
    char     file_name[NAME_MAX + 1];
    uint16_t depth;
} DirQueueEntry;

void dirview(const char *path);

#endif  // !DV_H
