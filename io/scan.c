#include "scan.h"

int scan(void)
{
    int c = getchar();

    if (c == EOF && ferror(stdin))
    {
        perror("error: cannot read the character");
    }

    return c;
}
