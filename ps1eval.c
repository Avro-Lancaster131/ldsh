#include <stdio.h>
#include <string.h>
#include "ps1eval.h"
#include "ps1vars.h"
#include <stdlib.h>
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

void ps1eval() {
    place = 0;
    for (i = 0; ps1[i] != '\0'; ++i) {
        if (ps1[i] == '\\') {
            if (ps1[i + 1] == ps1array(1)[0]) {
                pstoken[place] = strdup(getenv("USER"));
                pstoken[place + 1] = strdup("@ldsh");
                place = place + 2;
            }
            if (ps1[i + 1] == ps1array(2)[0] && ps1[i + 2] != ps1array(17)[1]) {
                if (cwd != NULL) {
                    pstoken[place] = strdup(cwd);
                    ++place;
                } else {
                    pstoken[place] = strdup("?");
                    ++place;
                }
            }
            if (ps1[i + 1] == ps1array(3)[0]) {
                pstoken[place] = strdup("$");
                ++place;
            }
            if (ps1[i + 1] == ps1array(4)[0] && ps1[i + 2] == ps1array(4)[1] && ps1[i + 3] == ps1array(4)[2]) {
                pstoken[place] = strdup("\033[31m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(5)[0] && ps1[i + 2] == ps1array(5)[1] && ps1[i + 3] == ps1array(5)[2]) {
                pstoken[place] = strdup("\033[32m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(6)[0] && ps1[i + 2] == ps1array(6)[1] && ps1[i + 3] == ps1array(6)[2]) {
                pstoken[place] = strdup("\033[34m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(7)[0] && ps1[i + 2] == ps1array(7)[1] && ps1[i + 3] == ps1array(7)[2]) {
                pstoken[place] = strdup("\033[0m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(8)[0] && ps1[i + 2] == ps1array(8)[1] && ps1[i + 3] == ps1array(8)[2]) {
                pstoken[place] = strdup("\033[93m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(9)[0] && ps1[i + 2] == ps1array(9)[1] && ps1[i + 3] == ps1array(9)[2]) {
                pstoken[place] = strdup("\033[35m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(10)[0] && ps1[i + 2] == ps1array(10)[1] && ps1[i + 3] == ps1array(10)[2]) {
                pstoken[place] = strdup("\033[36m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(11)[0] && ps1[i + 2] == ps1array(11)[1] && ps1[i + 3] == ps1array(11)[2] && ps1[i + 4] == ps1array(11)[3]) {
                pstoken[place] = strdup("\033[91m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(12)[0] && ps1[i + 2] == ps1array(12)[1] && ps1[i + 3] == ps1array(12)[2] && ps1[i + 4] == ps1array(12)[3]) {
                pstoken[place] = strdup("\033[94m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(13)[0] && ps1[i + 2] == ps1array(13)[1] && ps1[i + 3] == ps1array(13)[2] && ps1[i + 4] == ps1array(13)[3]) {
                pstoken[place] = strdup("\033[92m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(14)[0] && ps1[i + 2] == ps1array(14)[1] && ps1[i + 3] == ps1array(14)[2] && ps1[i + 4] == ps1array(14)[3]) {
                pstoken[place] = strdup("\033[93m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(15)[0] && ps1[i + 2] == ps1array(15)[1] && ps1[i + 3] == ps1array(15)[2] && ps1[i + 4] == ps1array(15)[3]) {
                pstoken[place] = strdup("\033[95m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(16)[0] && ps1[i + 2] == ps1array(16)[1] && ps1[i + 3] == ps1array(16)[2] && ps1[i + 4] == ps1array(16)[3]) {
                pstoken[place] = strdup("\033[96m");
                colour = 1;
                ++place;
            }
            if (ps1[i + 1] == ps1array(17)[0] && ps1[i + 2] == ps1array(17)[1] && ps1[i + 3] == ps1array(17)[2]) {
                pstoken[place] = strdup("\033[97m");
                colour = 1;
                ++place;
            }
        }
    }
}
