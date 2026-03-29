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
		"Command neither implemented nor found.", "echo", "exit", "cd", "--version", "help", "PS1"
	};
	return (n < 1 || n > 7) ? cmds[0] : cmds[n];
}

char *ps1array(int n) {
	static char *back[] = {
		"Not supported", "u", "w", "$", "red", "grn", "blu", "clr", "yel", "mag", "cyn", "bred", "bblu", "bgrn", "byel", "bmag", "bcyn", "wht"
		};
	return (n < 1 || n > 17) ? back[0] : back[n];
}

struct termios orig_termios;
void ExitRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void TermRawMode() {
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(ExitRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
//variables to declare before main()
char buffer[2049];
int pos, i, f;
char esc[4];
char *ps2;
char *ps1;
char cmd;
int colour, g;
ssize_t n;
char *clr[15];
int place;
char *pstoken[len];
char *history[100];
int hpos, hcount;
int updownarg;
char *cwd;

void ps1eval() {
	cwd = getcwd(NULL, 0);
	place = 0;
	g = 0;
        g = 0;
	colour = 0;
	for (i = 0; i < 15; ++i) {
                clr[i] = NULL;
        }
	for (i = 0; i < len; ++i) {
                pstoken[i] = NULL;
        }
	for (i = 0; i < len; ++i) {
		if (ps1[i] == '\\') {
			if (ps1[i + 1] == ps1array(1)[0]) {
				pstoken[place] = getenv("USER");
				pstoken[place + 1] = "@ldsh";
				place = place + 2;
			}
			if (ps1[i + 1] == ps1array(2)[0] && ps1[i + 2] != ps1array(17)[1]) {
				pstoken[place] = strdup(cwd);
				free(cwd);
				++place;
			}
			if (ps1[i + 1] == ps1array(3)[0]) {
				pstoken[place] = "$";
				++place;
			}
			if (ps1[i + 1] == ps1array(4)[0] && ps1[i + 2] == ps1array(4)[1] && ps1[i + 3] == ps1array(4)[2]) {
				clr[g] = "\033[31m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(5)[0] && ps1[i + 2] == ps1array(5)[1] && ps1[i + 3] == ps1array(5)[2]) {
				clr[g] = "\033[32m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(6)[0] && ps1[i + 2] == ps1array(6)[1] && ps1[i + 3] == ps1array(6)[2]) {
				clr[g] = "\033[34m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(7)[0] && ps1[i + 2] == ps1array(7)[1] && ps1[i + 3] == ps1array(7)[2]) {
				clr[g] = "\033[0m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(8)[0] && ps1[i + 2] == ps1array(8)[1] && ps1[i + 3] == ps1array(8)[2]) {
				clr[g] = "\033[93m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(9)[0] && ps1[i + 2] == ps1array(9)[1] && ps1[i + 3] == ps1array(9)[2]) {
				clr[g] = "\033[35m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(10)[0] && ps1[i + 2] == ps1array(10)[1] && ps1[i + 3] == ps1array(10)[2]) {
				clr[g] = "\033[36m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(11)[0] && ps1[i + 2] == ps1array(11)[1] && ps1[i + 3] == ps1array(11)[2] && ps1[i + 4] == ps1array(11)[3]) {
				clr[g] = "\033[91m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(12)[0] && ps1[i + 2] == ps1array(12)[1] && ps1[i + 3] == ps1array(12)[2] && ps1[i + 4] == ps1array(12)[3]) {
				clr[g] = "\033[94m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(13)[0] && ps1[i + 2] == ps1array(13)[1] && ps1[i + 3] == ps1array(13)[2] && ps1[i + 4] == ps1array(13)[3]) {
				clr[g] = "\033[92m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(14)[0] && ps1[i + 2] == ps1array(14)[1] && ps1[i + 3] == ps1array(14)[2] && ps1[i + 4] == ps1array(14)[3]) {
				clr[g] = "\033[93m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(15)[0] && ps1[i + 2] == ps1array(15)[1] && ps1[i + 3] == ps1array(15)[2] && ps1[i + 4] == ps1array(15)[3]) {
				clr[g] = "\033[95m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(16)[0] && ps1[i + 2] == ps1array(16)[1] && ps1[i + 3] == ps1array(16)[2] && ps1[i + 4] == ps1array(16)[3]) {
				clr[g] = "\033[96m";
				colour = 1;
				++place;
				++g;
			}
			if (ps1[i + 1] == ps1array(17)[0] && ps1[i + 2] == ps1array(17)[1] && ps1[i + 3] == ps1array(17)[2]) {
				clr[g] = "\033[97m";
				colour = 1;
				++place;
				++g;
			}
		}
	}
}
void ps1prt() {
	char *cwd = getcwd(NULL, 0);
	if (ps1 != NULL) {
		ps1eval();
	}
	if (g > 15) {
		printf("\033[31m!ERROR!\033[0m amount of colours is greater than 15!\n");
		exit(1);
	} else if (ps1 == NULL) {
	ps2 = "user@ldsh";
	}
	if (ps2 != NULL) {
		if (cwd == NULL) {
			cwd = "?";
			printf("%s%s ", ps2, cwd);
			fflush(stdout);
		} else {
			printf("%s%s ", ps2, cwd);
			fflush(stdout);
			free(cwd);
		}
	} else if (ps1 != NULL) {
		if (colour == 0) {
			for (f = 0; f < place; ++f) {
				if (pstoken[f] != NULL) {
					printf("%s", pstoken[f]);
				}
			}
		} else if (colour != 0) {
			g = 0;
			for (f = 0; f < place; ++f) {
				if (pstoken[f] != NULL) {
					printf("%s", pstoken[f]);
				} else if (pstoken[f] == NULL) {
					printf("%s", clr[g]);
					++g;
				}
			}
			printf("\033[0m");
			printf(" ");
		}
		fflush(stdout);
	}
}
void redraw() {
	n = write(STDOUT_FILENO, "\r", 1);
	if (updownarg == 1) {
		ps1prt();
		n = write(STDOUT_FILENO, "\033[K" , 3);
		n = write(STDOUT_FILENO, "\r", 1);
		ps1prt();
                for (i = 0; buffer[i] != '\0'; ++i) {
                        n = write(STDOUT_FILENO, &buffer[i], 1);
                }
                for (; i > pos; --i) {
                        n = write(STDOUT_FILENO, "\b", 1);
                }
                i = 0;
		updownarg = 0;
	} else if (updownarg == 0) {
		ps1prt();
		for (i = 0; buffer[i] != '\0'; ++i) {
			n = write(STDOUT_FILENO, &buffer[i], 1);
		}
		for (; i > pos; --i) {
			n = write(STDOUT_FILENO, "\b", 1);
		}
		i = 0;
	}
}
void initbuf() {
	for (pos = 0; pos < 2049; ++pos) {
		buffer[pos] = '\0';
	}
}
void reinitvar() {
        g = 0;
        colour = 0;

        for (i = 0; i < 4; ++i) {
	        esc[i] = '\0';
        }
        for (i = 0; i < 6; ++i) {
                clr[i] = NULL;
        }
        for (i = 0; i < len; ++i) {
                pstoken[i] = NULL;
	}
}

int main(int argc, char *argv[]) {
	TermRawMode();
        ps1 = getenv("PS1");
        ps2 = NULL;
        int nonewline;
        nonewline = 0;
        updownarg = 0;
        hpos = hcount = 0;
        for (i = 0; i < 100; ++i) {
                history[i] = NULL;
        }
	if (argc > 1) {
		if (strcmp(argv[1], builtins(4)) == 0) {
			printf("Limbo Dirt SHell v0.4.0-rc1 Copyright (C) 2026\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\nThis program comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to\nredistribute it under certain conditions\nSee LICENSE for more details.\nThere is NO WARRANTY, to the extent permitted by law.\n");
			exit(0);
		}
	}
	printf("Limbo Dirt SHell v0.4.0-rc1, There is NO WARRANTY, to the extent permitted by law.\nRead GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> for more info.\n");


	while (true) {
		init:
		reinitvar();
		if (nonewline == 0) {
			n = write(STDOUT_FILENO, "\n", 1);
		} else if (nonewline == 1) {
			nonewline = 0;
		}
		ps1prt();
		initbuf();
		//start input section

		pos = 0;
		while ((n = read(STDIN_FILENO, &cmd, 1)) == 1 && cmd != '\n' && cmd != '\r') {
                        if (cmd == 27) {
                              	esc[0] = cmd;
				n = read(STDIN_FILENO, &esc[1], 1);
				n = read(STDIN_FILENO, &esc[2], 1);
                                if (esc[1] == '[') {
                                        if (esc[2] == 'A') {
                                                //uparrow
						if (hpos > 0) {
							initbuf();
							strncpy(buffer, history[hpos - 1], sizeof(buffer));
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
			} else if (cmd == 3) {
				n = write(STDOUT_FILENO, "^C", 2);
				goto init;
                        } else if (cmd == 127 || cmd == 8 || cmd == '\b') {
				if (buffer[pos + 1] == '\0') {
					if (pos > 0) {
						--pos;
						buffer[pos] = '\0';
						n = write(STDOUT_FILENO, "\b \b", 3);
					}
				} else {
					if (pos > 0) {
						--pos;
						for (i = 1; buffer[pos + i] != '\0'; ++i) {
							buffer[pos + i - 1] = buffer[pos + i];
						}
						buffer[pos + i - 1] = '\0';
						n = write(STDOUT_FILENO, "\033[K", 3);
                                                redraw();
					}
				}
			} else if (cmd == 12) {
				n = write(STDOUT_FILENO, "\033[H\033[J", 6);
				ps1prt();
			} else if (cmd == 0x1C) {
				printf("\n\033[93m!Emergency Exit!\033[0m\n");
				fflush(stdout);
                                for (i = 0; i < 100; ++i) {
	                                free(history[i]);
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
				for (i = 1; i < 100 ; ++i) {
					history[i - 1] = history[i];
				}
				history[99] = strdup(buffer);
			}
		}
		if (strlen(buffer) > 2049) {
	       	        printf("\033[31m!ERROR!\033[0m: pos = %d\n", pos);
			exit(1);
		}
		buffer[strlen(buffer)] = '\0';
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

		if (token[0] == NULL) {
			nonewline = 0;
		} else if (strcmp(token[0], builtins(1)) == 0) {
			printf("\n");
			for (i = 1; i < x; ++i) {
				printf("%s ", token[i]);
			}

		} else if (strcmp(token[0], builtins(2)) == 0) {
                        n = write(STDOUT_FILENO, "\n", 1);
			ExitRawMode();
			goto finish;
		} else if (strcmp(token[0], builtins(3)) == 0) {
			if (token[1] != NULL) {
				n = write(STDOUT_FILENO, "\n", 1);
				nonewline = 1;
				if (chdir(token[1]) != 0) {
					perror("cd");
				}
			} else {
				if (chdir("/home") != 0) { //default buffer dir
					perror("cd");
				}
			}
		} else if (strcmp(token[0], builtins(5)) == 0) {
			printf("echo: prints all arguments past arg0\nexit: leaves the shell and either returns you to your previous shell, bash, zsh or fish, or if ldsh is your first shell, exits the terminal.\ncd: changes directory, checks if arg1 is a real directory, then changes to it\nldsh --version: prints the version of ldsh.\nhelp: prints this help menu inside of the shell\nPS1: sets the ps1 variable inside of the shell.\n");
		} else if (strcmp(token[0], builtins(6)) == 0) {
			if (token[1] != NULL) {
				ps2 = NULL;
				ps1 = strdup(token[1]);
				ps1eval();
			} else if (token[1] == NULL) {
				if (ps1 != NULL) {
                                	for (i = 0; i < len; ++i) {
						pstoken[i] = NULL;
                        	        }
					printf("%s\n", ps1);
				} else if (ps2 != NULL) {
					for (i = 0; i < len; ++i) {
						pstoken[i] = NULL;
					}
					printf("%s\n", ps2);
				}
			}
		} else {
			n = write(STDOUT_FILENO, "\n", 1);
			if ((pid = fork()) == 0) {
				execvp(token[0], token);
				perror(msg);
				_exit(1);
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
				} else {
					nonewline = 0;
				}
			}
		}
	fflush(stdout);
	}
	finish:
		for (i = 0; i < 100; ++i) {
			free(history[i]);
		}
}
