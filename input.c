#include "scan.h"

// enter on canonical mode for edit the input
void canonicalmode_on(struct termios *old) // need the old config of terminal
{
    struct termios new;

    tcgetattr(STDIN_FILENO, old);
    new = *old;

    new.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}