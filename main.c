#include <stdbool.h>
#include <stdio.h>

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
        int c = scan();

        if (c == EOF)
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
            unsigned char byte = (unsigned char)c;

            if (fwrite(&byte, 1, 1, fp) != 1)
            {
                perror("error: cannot write the character to the file");
                status = 1;
                done = true;
            }
        }
    }

    if (fclose(fp) != 0)
    {
        perror("error: cant close the file");
        status = 1;
    }

    return status;
}
