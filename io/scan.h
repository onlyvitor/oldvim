#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int scan(void);
void rawmode_on(struct termios *old);
void rawmode_off(struct termios *old);

#endif