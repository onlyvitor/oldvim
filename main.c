#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileio/fileio.h"
#include "io/scan.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = open_file(argv[1]);

    if (fp == NULL)
    {
        return 1;
    }

    bool done = false;
    int status = 0;

    while (!done)
    {
        char *buff = scan();

        if (buff == NULL)
        {
            if (ferror(stdin))
            {
                status = 1;
            }
            else
            {
                printf("\n[EOF]\n");
            }

            done = true;
        }
        else
        {
            size_t buff_len = strlen(buff);

            if (fwrite(buff, 1, buff_len, fp) != buff_len)
            {
                perror("error: cant write the line to the file");
                status = 1;
                done = true;
            }

            free(buff);
        }
    }

    if (fclose(fp) != 0)
    {
        perror("error: cant close the file");
        status = 1;
    }

    return status;
}
