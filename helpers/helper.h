#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include "../fileio/fileio.h"

/* Index of the first byte of the line that contains pos. */
size_t line_start_index(const char *data, size_t pos);

/* Redraw the buffer from start to the end and put the cursor back at cursor. */
int redraw_from(const EditorFile *ef, size_t start, size_t cursor);

#endif