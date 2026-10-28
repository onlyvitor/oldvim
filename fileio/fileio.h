#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <fcntl.h>

typedef struct EditorFile
{
    char *path;
    char *data;
    size_t len;
    size_t cap;
    mode_t mode;
    bool exist;
} EditorFile;

FILE *open_file(const char *filename);

#endif
