#include "ps1vars.h"
#include "ps1prt.h"
#include "ps1eval.h"
#include "redraw.h"
#include "shells.h"
#include "ldsh.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#define len 128

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
		"Command neither implemented nor found.", "echo", "exit", "cd", "--version", "help", "PS1", "history", "export", "unset", "alias"
	};
	return (n < 1 || n > 10) ? cmds[0] : cmds[n];
}

struct termios orig_termios;
void ExitRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void TermRawMode() {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(ExitRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
//variables to declare before main()
char cmd;
char *history[100];
int hpos, hcount;
int pid, g;
int export;
char *dblpath;
ssize_t n;
int apos = 0;

void initbuf() {
	for (pos = 0; pos < 4096; ++pos) {
		buffer[pos] = '\0';
	}
}

char *pointermerger(char *a, char *b) {
	free(dblpath);
	if (a != NULL && b != NULL) {
		n = strlen(a) + strlen(b) + 1;
		dblpath = malloc(n);
		if (!dblpath) return NULL;
		snprintf(dblpath, n, "%s%s", a, b);
		return dblpath;
	} else {
		return NULL;
	}
}
int status;
char msg[256];
int x;
int nonewline;
void tokeniser() {
	i = 2048;
	for (f = 0; f < i; ++f) {
		token[f] = NULL;
	}
	int intoken;
	intoken = 0;
	x = 0;
	//this creates intoken as not currently in word
	for (i = 0; buffer[i] != '\0'; ++i) {
		if (buffer[i] == '\"') { //double quote
			if (intoken == 0) {
				if (buffer[i + 1] != '\0' && buffer[i + 1] != '\"') {
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
					intoken = 1;
					token[x] = &buffer[i];
					while (buffer[i] != '\0' && buffer[i] != '\"') {
						++i;
					}
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
				}
				++x;
			} else {
				if (buffer[i + 1] != '\0' && buffer[i + 1] != '\"') {
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
					while (buffer[i] != '\0' && buffer[i] != '\"') {
						++i;
					}
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
				}
			}
		} else if (buffer[i] == '\'') { //single quote
			if (intoken == 0) {
				if (buffer[i + 1] != '\0' && buffer[i + 1] != '\'') {
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
					intoken = 1;
					token[x] = &buffer[i];
					while (buffer[i] != '\0' && buffer[i] != '\'') {
						++i;
					}
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
				}
				++x;
			} else {
				if (buffer[i + 1] != '\0' && buffer[i + 1] != '\'') {
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
					while (buffer[i] != '\0' && buffer[i] != '\'') {
						++i;
					}
					for (f = i; buffer[f] != '\0'; ++f) {
						buffer[f] = buffer[f + 1];
					}
				}
			}
		} else if (buffer[i] == '=') { //setting a var
			if (x == 1) {
				intoken = 0;
				buffer[i] = '\0';
				export = 1;
			}
		} else if (buffer[i] == '$') { //variables
			token[x] = &buffer[i + 1];
			while (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0') {
				++i;
			}
			buffer[i] = '\0';
			token[x] = getenv(token[x]);
			++x;
			--i;
			intoken = 0;
		} else if (buffer[i] == '#') { //comment
			while (buffer[i] != '\0') {
				++i;
			}
		} else if (buffer[i] == '~') { //HOME Variable
			token[x] = &buffer[i];
			++x;
			if (buffer[i + 1] != ' ' && buffer[i + 1] != '\t' && buffer[i + 1] != '\0') {
				token[x] = &buffer[i + 1];
				token[x - 1] = pointermerger(getenv("HOME"), token[x]);
				token[x + 1] = NULL;
			} else if (buffer[i + 1] == ' ' || buffer[i + 1] == '\t' || buffer[i + 1] == '\0') {
				token[x - 1] = getenv("HOME");
			}
			while (buffer[i + 1] != ' ' && buffer[i + 1] != '\t' && buffer[i + 1] != '\0') {
				++i;
			}
		} else if (buffer[i] == ' ' || buffer[i] == '\t') { //whitespace
			if (intoken) {
				intoken = 0;
			}
			buffer[i] = '\0';
		} else if (buffer[i] != ' ' && buffer[i] != '\t') { //normal characterwss
			if (!intoken) {
				intoken = 1;
				token[x] = &buffer[i];
				if (!x) {
					for (f = 0; f < 50 && alias[f] != NULL; ++f) { //checks for alias
						if (strcmp(token[0], alias[f]) == 0) {
							memmove(buffer + strlen(alias_tgt[f]), buffer + strlen(alias[f]), strlen(buffer) + strlen(alias[f]));
							memcpy(buffer, alias_tgt[f], strlen(alias_tgt[f]));
						}
					}
					f = 0;
				}
				token[x] = &buffer[i];
				++x;
			}
		}
	}
	token[x] = NULL;
	f = 0;
	sprintf(msg, "ldsh: %s", token[0]);
}
int commandparse() {
	if (token[0] == NULL) { //No command
		nonewline = 0;
		return 0;
	} else if (strcmp(token[0], builtins(1)) == 0) { // echo
		if (export == 0) {
			for (i = 1; i < x && token[i] != NULL; ++i) {
				printf("%s ", token[i]);
			}
			printf("\n");
			return 0;
		}
	} else if (strcmp(token[0], builtins(2)) == 0) { //exit
		n = write(STDOUT_FILENO, "\n", 1);
		ExitRawMode();
		for (i = 0; i < 100; ++i) {
			free(history[i]);
		}
		for (i = 0; i < len; ++i) {
			free(pstoken[i]);
		}
		for (i = 0; i < 50; ++i) {
			free(alias[i]);
			free(alias_tgt[i]);
		}
		free(dblpath);
		return -1;
	} else if (strcmp(token[0], builtins(3)) == 0) { //cd
		if (token[1] != NULL) {
			nonewline = 1;
			if (chdir(token[1]) != 0) {
				perror("cd");
			} else {
				return 0;
			}
		} else {
			if (chdir("/home") != 0) { //default buffer dir
				perror("cd");
			} else {
				return 0;
			}
		}
	} else if (strcmp(token[0], builtins(5)) == 0) { //help
		printf("echo: prints all arguments past arg0\nexit: leaves the shell and either returns you to your previous shell, bash, zsh or fish, or if ldsh is your first shell, exits the terminal.\ncd: changes directory, checks if arg1 is a real directory, then changes to it\nldsh --version: prints the version of ldsh.\nhelp: prints this help menu inside of the shell\nexport: sets a var (could also be used as dest=src)\nhistory: prints all previously typed commands.\nunset: unsets a variable from the env table.\nalias: sets one command to another, Usage: alias foo \"foo bar\" (notice the lack of an = sign)");
	} else if (strcmp(token[0], builtins(7)) == 0) { //history
		for (i = 0; i < hcount; ++i) {
			printf("%d\t%s\n", i, history[i]);
			fflush(stdout);
		}
		return 0;
	} else if (strcmp(token[0], builtins(8)) == 0) { //export
		if (token[2] != NULL) {
			setenv(token[1], token[2], 1);
			return 0;
		} else {
			setenv(token[1], "", 1);
			return 0;
		}
	} else if (export == 1) { //same deal, just checks for =
		if (token[1] != NULL) {
			setenv(token[0], token[1], 1);
			return 0;
		} else {
			setenv(token[0], "", 1);
			return 0;
		}
	} else if ((strcmp(token[0], builtins(9))) == 0) { //unset
		unsetenv(token[1]);
		return 0;
	} else if (strcmp(token[0], builtins(6)) == 0) { //PS1
		printf("\n\033[93m!WARNING!\033[0m PS1 as a command is deprecated and is no longer used!");
		return 1;
	} else if ((strcmp(token[0], builtins(10))) == 0) { //alias
		if (token[2] == NULL) {
			if (token[1] != NULL) {
				printf("alias usage: alias name \"target\"");
			} else {
				for (i = 0; i < 50 && alias[i] != NULL; ++i) {
					printf("%d\t%s > %s\n", i, alias[i], alias_tgt[i]);
					fflush(stdout);
				}
			}
			return 0;
		} else {
			if (strcmp("-d", token[1]) == 0) {
				f = atoi(token[2]);
				free(alias[f]);
				free(alias_tgt[f]);
				alias[f] = alias_tgt[f] = NULL;
			}
			alias[apos] = strdup(token[1]);
			alias_tgt[apos] = strdup(token[2]);
			++apos;
		}
	} else { //fork, ls, other commands
		if (f) {
			n = write(STDOUT_FILENO, "\n", 1);
		}
		if ((pid = fork()) == 0) {
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			ExitRawMode();
			execvp(token[0], token);
			perror(msg);
			_exit(1);
			return 1;
		}
		if (pid < 0) {
			printf("\033[31m!ERROR!\033[0m: fork() failed! Exiting...\n");
			return 1;
		}
		if (pid > 0) {
			wait(&status);
			if (WIFEXITED(status) == true) {
				if (WEXITSTATUS(status) > 0) {
					nonewline = 1;
				} else if (WEXITSTATUS(status) == 0) {
					nonewline = 1;
				}
				return 0;
			} else {
				nonewline = 0;
				return 0;
			}
			TermRawMode();
			return 0;
		}
	}
	return 0;
}
int main(int argc, char *argv[]) {
        ps2 = NULL;
        nonewline = 0;
        updownarg = 0;
        hpos = hcount = 0;
        for (i = 0; i < 100; ++i) {
                history[i] = NULL;
        }
	if (argc > 1) {
		if (strcmp(argv[1], builtins(4)) == 0) {
			printf("Limbo Dirt SHell v0.5.0 Copyright (C) 2026\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\nThis program comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to\nredistribute it under certain conditions\nSee LICENSE for more details.\nThere is NO WARRANTY, to the extent permitted by law.\n");
			exit(0);
		} else {
			shellscript(argv[1]);
			exit(0);
		}
	}
	printf("Limbo Dirt SHell v0.5.0, There is NO WARRANTY, to the extent permitted by law.\nRead GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> for more info.\n");
	dblpath = NULL;
	shellscript(pointermerger(getenv("HOME"), "/.ldshrc"));
	while (true) {
		init:
		TermRawMode();
		free(dblpath);
		dblpath = NULL;
		ssize_t n;
		if (nonewline == 0) {
			n = write(STDOUT_FILENO, "\n", 1);
		} else if (nonewline == 1) {
			nonewline = 0;
		}
		initbuf();
		initps1var();
		ps1prt();
		fflush(stdout);
		export = 0;
		//start input section
		pos = 0;
		while ((n = read(STDIN_FILENO, &cmd, 1)) == 1 && cmd != '\n' && cmd != '\r') {
                        if (cmd == 27) {
								char esc[4];
								for (i = 0; i < 4; ++i) {
									esc[i] = '\0';
								}
                              	esc[0] = cmd;
				n = read(STDIN_FILENO, &esc[1], 1);
				n = read(STDIN_FILENO, &esc[2], 1);
                                if (esc[1] == '[') {
                                        if (esc[2] == 'A') {
                                                //uparrow
						if (hpos > 0) {
							initbuf();
							strncpy(buffer, history[hpos - 1], sizeof(buffer) - 1);
							pos = strlen(buffer);
							--hpos;
							updownarg = 1;
							redraw();
						}
                                        }
                                        if (esc[2] == 'B') {
                                                //downarrow
						if (hpos < hcount - 1) {
							initbuf();
							strncpy(buffer, history[hpos + 1], sizeof(buffer));
							buffer[sizeof(buffer) - 1] = '\0';
							pos = strlen(buffer);
							++hpos;
							updownarg = 1;
							redraw();
						} else {
							initbuf();
							pos = 0;
							updownarg = 1;
							hpos = hcount;
							redraw();
						}
                                        }
                                        if (esc[2] == 'C') {
                                                //rightarrow
						if (pos < strlen(buffer)) {
							++pos;
							redraw();
						}
                                        }
                                        if (esc[2] == 'D') {
                                                //leftarrow
         					if (pos > 0) {
							n = write(STDOUT_FILENO, "\b", 1);
							--pos;
						}
                                        }
					if (esc[2] == 'H') {
						//Home
						pos = 0;
						redraw();
					}
					if (esc[2] == 'F') {
						//End
						pos = strlen(buffer);
						redraw();
					}
					if (esc[2] == '3') {
						//Delete
						n = read(STDIN_FILENO, &esc[3], 1);
						if (esc[3] == '~') {
							for (i = pos; buffer[i] != '\0'; ++i) {
								buffer[i] = buffer[i + 1];
							}
							buffer[i] = '\0';
							n = write(STDOUT_FILENO, "\033[K", 3);
							redraw();
						}
					}
                                }
			} else if (cmd == 3) { //^C
				n = write(STDOUT_FILENO, "^C", 2);
				goto init;
                        } else if (cmd == 127 || cmd == 8 || cmd == '\b') { //backspace
				if (buffer[pos] == '\0') {
					if (pos > 0) {
						--pos;
						buffer[pos] = '\0';
						redraw();
					}
				} else { //midline backspace
					if (pos > 0) {
						--pos;
						for (i = pos; buffer[i] != '\0'; ++i) {
							buffer[i] = buffer[i + 1];
						}
						n = write(STDOUT_FILENO, "\033[K", 3);
						redraw();
					}
				}
			} else if (cmd == 12) { //^L
				n = write(STDOUT_FILENO, "\033[H\033[J", 6);
				ps1prt();
			} else if (cmd == 0x1C) { /*Ctrl + \*/
				printf("\n\033[93m!Emergency Exit!\033[0m\n");
				fflush(stdout);
                                for (i = 0; i < 100; ++i) {
	                                free(history[i]);
					free(pstoken[i]);
                                }
				for (; i < len; ++i) {
					free(pstoken[i]);
				}
                                exit(1);
			} else {
				if (buffer[pos] == '\0') {
					n = write(STDOUT_FILENO, &cmd, 1);
	                                buffer[pos] = cmd;
        	                        ++pos;
				} else {
					i = strlen(buffer) - 1;
					for (; i >= pos; --i) {
						buffer[i + 1] = buffer[i];
					}
					buffer[pos] = cmd;
					++pos;
					redraw();
				}
			}
		}
		if (buffer[0] != '\0') {
			if (hcount < 100) {
				history[hcount] = strdup(buffer);
				++hcount;
				hpos = hcount;
			} else {
				free(history[0]);
				for (i = 1; i < 100 ; ++i) {
					history[i - 1] = strdup(history[i]);
				}
				history[99] = strdup(buffer);
			}
		}
		if (strlen(buffer) > 4096) {
			printf("\033[31m!ERROR!\033[0m: buffer overflow\n");
			exit(1);
		}
		buffer[strlen(buffer)] = '\0';
		//end input
		//begin processing
		tokeniser();
		printf("\n");
		int exitval = commandparse();
		nonewline = 1;
		fflush(stdout);
		if (exitval == -1) {
			break;
		}
	}
	ExitRawMode();
}
