#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int scan(void);
int rawmode_on(struct termios *old);
int rawmode_off(const struct termios *old);

#endif