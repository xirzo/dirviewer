#ifndef DV_H
#define DV_H

#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

#ifndef DIRSIZ
#define DIRSIZ 14
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_NAME PATH_MAX

typedef struct {
    char  name[PATH_MAX + 1];
    ino_t inode;
} DirEntry;

typedef struct {
    DIR*     dirp;
    DirEntry de;
} Dir;

Dir*      opendirectory(const char* dir);
DirEntry* readdirectory(Dir* d);
void      closedirectory(Dir* d);

void fsize(const char* name);
void print(const char* name);
void dirwalk(const char* dir, void (*func)(const char*));

#endif  // !DV_H
