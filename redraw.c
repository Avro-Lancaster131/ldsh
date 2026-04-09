#include "ps1vars.h"
#include "ps1prt.h"
#include "ps1eval.h"
#include "redraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*  Limbo Dirt Shell, Minimal shell that processes input and output
 C opyright (C) 2026 Michael Bogicevic                                     *

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */

ssize_t z;

void redraw() {
    z = write(STDOUT_FILENO, "\r", 1);
    if (updownarg == 1) {
        z = write(STDOUT_FILENO, "\033[K" , 3);
        z = write(STDOUT_FILENO, "\r", 1);
        ps1prt();
        i = strlen(buffer);
        z = write(STDOUT_FILENO, buffer, i);
        for (; i > pos; --i) {
            z = write(STDOUT_FILENO, "\b", 1);
        }
        i = 0;
        updownarg = 0;
    } else if (updownarg == 0) {
        z = write(STDOUT_FILENO, "\033[K", 3);
        z = write(STDOUT_FILENO, "\r", 1);
        ps1prt();
        i = strlen(buffer);
        z = write(STDOUT_FILENO, buffer, i);
        for (; i > pos; --i) {
            z = write(STDOUT_FILENO, "\b", 1);
        }
        i = 0;
    }
    fflush(stdout);
}
