#include "ps1vars.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

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

char *ps1array(int n) {
    static char *back[] = {
        "Not supported", "u", "w", "$", "red", "grn", "blu", "clr", "yel", "mag", "cyn", "bred", "bblu", "bgrn", "byel", "bmag", "bcyn", "wht"
    };
    return (n < 1 || n > 17) ? back[0] : back[n];
}


char *cwd;
int i;
int f;
char *ps1;
char *ps2;
int place;
char *pstoken[128];
int colour;
char buffer[4097];
int pos;
int updownarg;
char *token[2048];
char *alias[50] = {NULL};
char *alias_tgt[50] = {NULL};

void cwdupdate(void) {
    cwd = malloc(PATH_MAX);
    cwd = getcwd(cwd, PATH_MAX);
}

void initps1var(void) {
    ps1 = getenv("PS1");
    place = pos = i = f = colour = 0;
    ps1 = NULL;
    ps2 = NULL;
    for (i = 0; i < 15; ++i) {
        buffer[i] = '\0';
    }
    for (i = 0; i < 128; ++i) {
        pstoken[i] = NULL;
    }
}

void cwdclean(void) {
    free(cwd);
    cwd = NULL;
}
