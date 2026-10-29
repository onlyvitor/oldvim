#include "scan.h"

int scan(void)
{
    int c = getchar();

    if (c == EOF && ferror(stdin))
    {
        perror("error: cannot read the character");
    }
    else if (c == 0x08)
    {
        c = KEY_BACKSPACE;
    }

    return c;
}
