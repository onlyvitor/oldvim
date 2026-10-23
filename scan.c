#include "scan.h"

char *scan()
{
    char *line = NULL;
    size_t size = 0;

    if (getline(&line, &size, stdin) == -1)
    {
        perror("error: cant read line");
        free(line);
        return NULL;
    }

    return line;
}
