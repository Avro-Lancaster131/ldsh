#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*  Limbo Dirt Shell, Minimal shell that processes input and output
    Copyright (C) 2026 Michael Bogicevic

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

char *builtins(int n) {
	static char *cmds[] = {
		"Command neither implemented nor found.", "echo", "exit", "cd", "--version"
	};
	return (n < 1 || n > 4) ? cmds[0] : cmds[n];
}

int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], builtins(4)) == 0) {
			printf("Limbo Dirt SHell v0.1.1 Copyright (C) 2026\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\nThis program comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to\nredistribute it under certain conditions\nSee LICENSE for more details.\nThere is NO WARRANTY, to the extent permitted by law.\n");
		exit(0);
		}
	}
	printf("Limbo Dirt SHell v0.1.1, There is NO WARRANTY, to the extent permitted by law.\nRead GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> for more info.\n");

       	char buffer[1025];
        int cmd, pos, i;
	while (true) {

		char *cwd = getcwd(NULL, 0);
		printf("user@ldsh%s ", cwd);
		free(cwd);

		//init pos and buffer for input
		for (pos = 0; pos < 1025; ++pos) {
			buffer[pos] = '\0';
		}
		 //start input section

		pos = 0;
		while ((cmd = getchar()) != '\n') {
			buffer[pos] = cmd;
			++pos;
		}

		if (pos > 1024) {
	       	        printf("!ERROR!: pos = %d\n", pos);
			break;
			return 1;
		}

		buffer[pos] = '\0';
		//end input

		//begin processing
		char *token[pos];
		for (i = 0; i < pos; ++i) {
			token[i] = NULL;
		}

		int intoken;
		int x;
		intoken = 0;
		x = 0;
		//this creates intoken as not currently in word

		for (i = 0; buffer[i] != '\0'; ++i) {
			if (buffer[i] != ' ' && buffer[i] != '\t') {
				if (!intoken) {
					token[x] = &buffer[i];
					intoken = 1;
					++x;
				}
			}
			if (buffer[i] == ' ' || buffer[i] == '\t') {
				if (intoken) {
					intoken = 0;
					buffer[i] = '\0';
				}
			}
		}
		token[x] = NULL;

		//creates variables for use during command processing
		int pid;
		int status;
		char msg[256];
		sprintf(msg, "ldsh: %s", token[0]);

		if (token[0] == NULL) { //skips the other if checks if token is NULL
		} else if (strcmp(token[0], builtins(1)) == 0) {
			for (i = 1; i < x; ++i) {
				printf("%s ", token[i]);
			}
			printf("\n");
		} else if (strcmp(token[0], builtins(2)) == 0) {
			goto finish;
		} else if (strcmp(token[0], builtins(3)) == 0) {
			if (token[1] != NULL) {
				if (chdir(token[1]) != 0) {
					perror("cd");
				}
			} else {
				if (chdir("/home") != 0) { //default buffer dir
					perror("cd");
				}
			}
		} else {
			if ((pid = fork()) == 0) {
				execvp(token[0], token);
				perror(msg);
				_exit(1);
			}
			if (pid < 0) {
				printf("!ERROR!: fork() failed! Exiting...\n");
				return 1;
			}
			if (pid > 0) {
				wait(&status);
			}
		}
	}

	finish:
	exit(0);
}
