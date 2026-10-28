#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct EditorFile
{
    char *path;
    char *data;
    size_t len;
    size_t cap;
    mode_t mode;
    bool exist;
} EditorFile;

EditorFile *open_file(const char *filename);
int append_bytes(EditorFile *ef, char const *s, size_t n);

#endif
