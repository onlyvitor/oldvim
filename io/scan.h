/* Feature-test macro: must be defined before any system header. */
#define _POSIX_C_SOURCE 200809L

#ifndef SCAN_H
#define SCAN_H

#include "key.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int scan(void);
int rawmode_on(void);
int rawmode_off(void);

#endif
