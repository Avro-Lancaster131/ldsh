#include "ps1vars.h"
#include "ps1prt.h"
#include "ps1eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define len 128

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

void ps1prt() {
    cwdupdate();
    ps1 = getenv("PS1");
    if (ps1 != NULL) {
        ps1eval();
        if (colour == 0) {
            for (f = 0; f < place; ++f) {
                if (pstoken[f] != NULL) {
                    printf("%s", pstoken[f]);
                }
            }
        } else if (colour != 0) {
            for (f = 0; f < place; ++f) {
                if (pstoken[f] != NULL) {
                    printf("%s", pstoken[f]);
                }
            }
            printf("\033[0m");
        }
        printf(" ");
    } else if (ps1 == NULL) {
        ps2 = "user@ldsh";
        if (cwd == NULL) {
            cwd = strdup("?");
            printf("%s%s ", ps2, cwd);
        } else {
            printf("%s%s ", ps2, cwd);
        }
    }
    for (i = 0; i < len; ++i) {
        free(pstoken[i]);
        pstoken[i] = NULL;
    }
    cwdclean();
    fflush(stdout);
}
