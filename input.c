#include "scan.h"

// enter on canonical mode for edit the input
void canonicalmode_on(struct termios *old) // need the old config of terminal
{
    struct termios new;

    tcgetattr(STDIN_FILENO, old);
    new = *old;

    // desativa o buffer de linha e a exibiçao automatica na tela
    new.c_lflag &= ~(ICANON | ECHO);

    // aplica as novas configiurações
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

// return to the default config of terminal
void canonicalmode_off(struct termios *old)
{
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}