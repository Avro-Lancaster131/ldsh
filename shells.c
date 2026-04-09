#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ps1vars.h"
#include "ps1prt.h"
#include "ps1eval.h"
#include "ldsh.h"
#include "shells.h"

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

void shellscript(const char *file) {
    f = 0;
    FILE *fp = fopen(file, "r");
    if (!fp) {
        return;
    }
    while (fgets(buffer, sizeof(buffer) - 1, fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        f = 1;
        tokeniser();
        commandparse();
        for (i = 0; i > 2048; ++i) {
            token[i] = NULL;
        }
    }
}
