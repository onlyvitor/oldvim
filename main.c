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

    EditorFile *ef = open_file(argv[1]);

    if (ef == NULL)
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
            char byte = (char)c;

            if (append_bytes(ef, &byte, 1) != 0)
            {
                perror("error: cannot write the character to the file");
                status = 1;
                done = true;
            }
        }
    }

    return status;
}
