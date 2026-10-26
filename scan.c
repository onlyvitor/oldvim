#define _POSIX_C_SOURCE 200809L

#include "scan.h"

char *scan(void)
{
    char *line = NULL;
    size_t size = 0;

    if (getline(&line, &size, stdin) == -1)
    {
        /* getline retorna -1 tanto no fim do arquivo quanto em erro */
        if (ferror(stdin))
        {
            perror("error: cant read line");
        }

        free(line);
        return NULL;
    }

    return line;
}
